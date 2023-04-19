#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buf.h"

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define BUF_MAX 30

static char buf[BUF_MAX];
static char rcli_out_buf[BUF_MAX];

static ctrlBuf_s bufStruct;

#define RCLI_PROMPT_STR   "\r\e[38;5;15m\e[1mRVM>\e[0m "
#define RCLI_PROMPT_SHIFT 5    /* strlen of RCLI_PROMPT_STR */

char RcliTransferChar(const char ch)
{
    if (ch != '\0')
        printf("%c", ch);

    return 0;
}

void RcliReceiveChar(char ch)
{
}

static char RcliTransferStr(char * const pBuf, unsigned char len)
{
    char *buf = pBuf;

    if (len > BUF_MAX)
    {
        len = BUF_MAX;
    }

    if (buf == NULL)
    {
        return 1;
    }
    
    while(len--)
    {
        if (RcliTransferChar(*buf++) != 0)
        {
            return 1;
        }
    }

    return 0;
}

void RcliInit(void)
{
    buf_init(&bufStruct, buf, BUF_MAX);
    sprintf(rcli_out_buf, "Welcome (RCli)\n");
    RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
    
    sprintf(rcli_out_buf, "%s", RCLI_PROMPT_STR);
    RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
}

char raw_buf[BUF_MAX];
char operate_buf[BUF_MAX];

#define RCLI_ARGS_LENGTH    10
#define RCLI_ARGS_MAX_COUNT 10
char echo_func_cmd(unsigned char args, void* argv)
{
    printf("Hello from echo callback!\r\n");

    return 0;
}

char cmd_func_cmd(unsigned char args, void* argv)
{
//    printf("Hello from cmd callback!%d - %s\r\n", args, (char *)(argv)+RCLI_ARGS_LENGTH*1);

    return 0;
}

char status_func_cmd(unsigned char args, void* argv)
{
    printf("params=%d\r\n", args);

    if (args == 2)
    {
        if (strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 1, "get") == 0)
        {
            printf("[!] GET\r\n");
            return 0;
        }
    }

    if (args ==3)
    {
        if (strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 1, "set") == 0)
        {
            int val = atoi((char*)(argv) + RCLI_ARGS_LENGTH * 2);
            printf("[!] SET %d\r\n", val);
            return 0;
        }
    }
    return -1;
}

char help_func_cmd(unsigned char args, void* argv);

typedef char (*cb_t)(unsigned char args, void* argv);
typedef struct
{
    char argc;
    char ** argv;
    cb_t func;
} rcli_cmd_t;

rcli_cmd_t rcli_commands[] = 
{
    { 3, (char*[]){ "cmd", "ddd", "q", NULL }, cmd_func_cmd },
    { 1, (char*[]){ "echo", NULL}, echo_func_cmd },
    { 3, (char*[]){ "status", "set", "get", NULL}, status_func_cmd },
    { 1, (char*[]){ "help", NULL}, help_func_cmd },
    { 1, (char*[]){ "?", NULL}, help_func_cmd }
};

#define RCLI_CMD_SIZE ( sizeof(rcli_commands) / sizeof(rcli_cmd_t) )

char help_func_cmd(unsigned char args, void* argv)
{
    for (int i = 0; i < RCLI_CMD_SIZE - 1; i++)
    {
        printf("  %s\r\n", (char*)rcli_commands[i].argv[0]);
    }
    return 0;
}

char rcli_parse_cmd(ctrlBuf_s *bufStruct)
{
    char pos = -1;
    char res = -1;
    unsigned char n = 0;
    // printf("---\n%s\n---\n", bufStruct->pBuf);
    
    res = buf_get_count_params(bufStruct); 
    
    if (res == -1)
    {
        DEBUG_PRINT(("%s: Error get count params\n", __func__));
        return -1;
    }

    if (res == 0)
    {
        return 0;
    }

    // Get first word position
    pos = buf_get_pos_n_word(bufStruct, 0);

    if (pos == -1)
    {
        DEBUG_PRINT(("%s: Error get %d word position\n", __func__, n));
        return -1;
    }

    int i = 0; 
    for (i = 0; i < RCLI_CMD_SIZE; i++)
    {
        if (rcli_commands[i].argv == NULL)
        {
            DEBUG_PRINT(("%s: rcli command is NULL. Check rcli_commands %d line.\n", __func__, i + 1));
            continue;
        }

        char * pCmdPos = strstr(bufStruct->pBuf, rcli_commands[i].argv[0]);
        if (pCmdPos != bufStruct->pBuf + pos)
        {
            // printf("NOT CALLBACK\r\n");
        }
        else
        {
            unsigned char args = 0; 
            char arr[RCLI_ARGS_MAX_COUNT][RCLI_ARGS_LENGTH];

            char * ptr = bufStruct->pBuf;
            char * head = ptr;
            do
            {
                while(*ptr == 32)
                {
                    ptr++;
                }
                head = ptr;
                while(*ptr != 32 && *ptr != '\0')
                {
                    ptr++;
                }
                strncpy(arr[args], head, ptr - head);
                arr[args][ptr-head] = '\0';
                if (head != ptr)
                {
                    args++;
                }
            }
            while (*ptr++);

            if (rcli_commands[i].func != NULL)
            {
                if (rcli_commands[i].func(args, (char**)arr) == -1)
                    printf("ERROR\n");
            }
            break;
        }
    }

    if (i >= RCLI_CMD_SIZE)
    {
        printf("Command not found\r\n");
        return -1;
    }

    return 0;
}

void rcli_parse_buf(char * buf)
{
    char *p_tmp;
    char pos = 0;
    // block uart
    if (buf == NULL)
        return;
    
    if (buf[0]=='\0')
        return;

    p_tmp = buf;

    do
    {
        /*if (*p_tmp >= 32 && *p_tmp <= 126)
        {
            buf_add(&bufStruct, *p_tmp, 0);
            //printf("%c; ", *p_tmp);
        }*/

        /*if (*p_tmp == 13)
        {*/
            if (rcli_parse_cmd(&bufStruct) == -1)
            {
            buf_debug(bufStruct);
                buf_clear(&bufStruct);
                break;
            }
            buf_debug(bufStruct);
            buf_clear(&bufStruct);
        //}
    }
    while(*p_tmp++);
    buf[0] = '\0';
    //unblock uart
}

void uart_rx(void)
{
    //sprintf(raw_buf, " status   get \n status set 3\n");
    int i = 0;
    int c;
    int esc_status = 0;
    system ("/bin/stty raw");
    system("stty -echo");
    while((c=getchar())!= '.') 
    {
        //printf(".%d.", c);

        switch(esc_status)
        {
            case 0:
                switch(c)
                {
                    case 27: esc_status = 1; break;
                    default: esc_status = 0; break;
                }
                break;
            case 1:
                switch(c)
                {
                    case 91: esc_status = 2; break;
                    default: esc_status = 0; break;
                }
                break;
            case 2:
                switch(c)
                {
                    case 'D': esc_status = 3; break;
                    case 'C': esc_status = 4; break;
                    default:  esc_status = 5; break;
                }
                break;
            default: 
                esc_status = 0;
                break;
        }

        switch(esc_status)
        {
            case 0:
                break;
            case 3:
                esc_status = 0;
                if (bufStruct.cur_pos > bufStruct.end)
                    continue;
                buf_move_cur(&bufStruct, BUF_CUR_LEFT);
                printf("\e[%dD", 1);
                continue;
            case 4:
                esc_status = 0;
                if (bufStruct.cur_pos == 1)
                    continue;
                buf_move_cur(&bufStruct, BUF_CUR_RIGHT);
                printf("\e[%dC", 1);
                continue;
            case 5:
                esc_status = 0;
                continue;
            default:
                continue;
        }

        if (c == 127)
        {
            if (bufStruct.end == 0)
                continue;
            buf_move_cur(&bufStruct, BUF_CUR_LEFT);
            buf_del(&bufStruct);
            printf("\e[%dD ", 1);
            printf("\e[%dD", 1);
            continue;
        }
        if ((c >= 32 && c < 127))
        {
            buf_add(&bufStruct, c, 0);

            if(bufStruct.cur_pos != 1)
            {
                char pos = bufStruct.end - bufStruct.cur_pos;
                if (pos > 0)
                {
                    printf("\e[%dD", pos);
                }
                printf("%s", bufStruct.pBuf);
                printf("\e[%dD", bufStruct.cur_pos - 1);
            }
            else
                putchar(c);

            if (i >= BUF_MAX)
                continue;
            i++;
        }
        if (c == 13)
        {
            printf("\r\n");
            rcli_parse_buf(bufStruct.pBuf);

            //buf_debug(bufStruct);
            buf_clear(&bufStruct);
            
            sprintf(rcli_out_buf, "%s", RCLI_PROMPT_STR);
            RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
        }
    }
    system ("/bin/stty cooked");
    system("stty echo");
   // Call parse after timeout
}


int main(void)
{


//    char ** p = (char**)rcli_commands[0].params;
//    printf(">%ld<\n", sizeof(p)/sizeof(p[0]));
    
//    for (char **arg = rcli_commands[0].argv; *arg != NULL; ++arg)
//    {
//        printf("Arg %s\n", *arg);
//    }

    RcliInit();
    raw_buf[0] = '\0';
    uart_rx();

    printf("\n\nBoot...\n");
    return 0; 


    char b[128];
    int i = 0;
//    sprintf(b, "Hello%s world\n", "\e[X");
    sprintf(b, "Hello world\n");
    while(b[i] != '\n')
    {
        //if (b[i] >= 32 && b[i] <= 254 && b[i] != 127)
        //{
            RcliTransferChar(b[i]);
            buf_add(&bufStruct, b[i], 0);
        //}
        i++;
    }
    /* parse */

    /* next command */
    sprintf(rcli_out_buf, "\n%s", RCLI_PROMPT_STR);
    RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));

    printf("\n===========\n");

    buf_debug(bufStruct);
    //return 0;

    buf_add(&bufStruct, 'a', 0);
    buf_add(&bufStruct, 'b', 0);
    buf_add(&bufStruct, 'c', 0);
    buf_add(&bufStruct, 'd', 0);
    buf_add(&bufStruct, 'e', 0);
    buf_move_cur(&bufStruct, BUF_CUR_LEFT);
    buf_move_cur(&bufStruct, BUF_CUR_LEFT);
    buf_move_cur(&bufStruct, BUF_CUR_LEFT);
    buf_move_cur(&bufStruct, BUF_CUR_LEFT);
    buf_add(&bufStruct, '3', 0);
    buf_move_cur(&bufStruct, BUF_CUR_RIGHT);
    buf_move_cur(&bufStruct, BUF_CUR_RIGHT);
    buf_move_cur(&bufStruct, BUF_CUR_RIGHT);
    buf_add(&bufStruct, '4', 0);
    buf_move_cur(&bufStruct, BUF_CUR_HOME);
    buf_add(&bufStruct, 'H', 0);
    buf_move_cur(&bufStruct, BUF_CUR_END);
    buf_add(&bufStruct, 'E', 0);

    printf("%s\n", buf);

    buf_move_cur(&bufStruct, BUF_CUR_HOME);
    buf_del(&bufStruct);
    printf("%s\n", buf);

    printf("Clear buf...\n");
    buf_clear(&bufStruct);
    printf("%s\nDone\n", buf);
    buf_debug(bufStruct);

    buf_add(&bufStruct, 'a', 0);
    buf_add(&bufStruct, 'b', 0);
    buf_add(&bufStruct, 'c', 0);
    printf("%s\n", buf);

    return 0;

    int c;
    system ("/bin/stty raw");
    system("stty -echo");
    while((c=getchar())!= '.') 
    {
        if (c >= 32 && c < 127)
        putchar(c);
        if (c == '\033')
        {
            c = getchar();
            if (c == '[')
            {
                c = getchar();
                if (c == 'D')
                {
                    //printf("\e[%dD\e[K", 5);
                    printf("\e[%dD", 1);
                }
                if (c == 'C')
                {
                    printf("\e[%dC", 1);
                }
            }
        }
    }
    system ("/bin/stty cooked");
    system("stty echo");
    return 0;
}
