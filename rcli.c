#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "version.h"
#include "buf.h"

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define BUF_MAX 128

static char buf[BUF_MAX];
static char rcli_out_buf[BUF_MAX];

static ctrlBuf_s bufStruct;
static char isSetColor;

#define RCLI_STR_WARNING(str)           (isSetColor ? "\e[38;5;11m" str "\e[0m" : str)
#define RCLI_STR_ERROR(str)             (isSetColor ? "\e[38;5;1m" str "\e[0m" : str)
#define RCLI_PROMPT_SET_COLOR_STR       "\r\e[38;5;15m\e[1mconsole>\e[0m "
#define RCLI_PROMPT_CLEAR_COLOR_STR     "\rconsole> "
#define RCLI_PROMPT(str)                (isSetColor ? "\e[38;5;15m\e[1m" str "\e[0m" : str)
#define RCLI_PROMPT_STR                 RCLI_PROMPT("\rConsole> ") 
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
    isSetColor = 1;
    buf_init(&bufStruct, buf, BUF_MAX);
    sprintf(rcli_out_buf, "Welcome (RCli %s)\n", RCLI_VERSION);
    RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
    
    sprintf(rcli_out_buf, "%s", RCLI_PROMPT_STR);
    RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
}

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
char console_func_cmd(unsigned char args, void* argv);

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
    { 1, (char*[]){ "console", NULL}, console_func_cmd },
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

char console_func_cmd(unsigned char args, void* argv)
{
    char * help_str = 
"Usage: console [OPTION]\r\n\r\n\
  color set|clear\t\tSet color theme\r\n\r\n";

    if (args == 1)
    {
        printf("%s", help_str);
        return 0;
    }

    if (args == 2)
    {
        if (strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 1, "color") == 0 ||
            strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 1, "help") == 0 ||
            strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 1, "?") == 0)
        {
            printf("%s", help_str);
            return 0;
        }
    }

    if (args == 3)
    {
        if (strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 1, "color") == 0)
        {
            if (strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 2, "set") == 0)
            {
                isSetColor = 1;
                printf("Example: %s %s %s\r\n", RCLI_STR_ERROR("Error"), RCLI_STR_WARNING("Warning"), RCLI_PROMPT("Prompt"));
                return 0;
            }
            if (strcmp((char*)(argv) + RCLI_ARGS_LENGTH * 2, "clear") == 0)
            {
                isSetColor = 0;
                printf("Example: %s %s %s\r\n", RCLI_STR_ERROR("Error"), RCLI_STR_WARNING("Warning"), RCLI_PROMPT("Prompt"));
                return 0;
            }
        }
    }

    printf("Bad params\r\n");
    return -1;
}

char rcli_parse_cmd(ctrlBuf_s bufStruct)
{
    char pos = -1;
    char res = -1;
    unsigned char n = 0;
    char *pBuf = NULL;
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

        pBuf = buf_get_buf(bufStruct);
        char * pCmdPos = strstr(pBuf, rcli_commands[i].argv[0]);
        if (pCmdPos != pBuf + pos)
        {
            // printf("NOT CALLBACK\r\n");
        }
        else
        {
            unsigned char args = 0; 
            char arr[RCLI_ARGS_MAX_COUNT][RCLI_ARGS_LENGTH];

            char * head = pBuf;
            do
            {
                while(*pBuf == 32)
                {
                    pBuf++;
                }
                head = pBuf;
                while(*pBuf != 32 && *pBuf != '\0')
                {
                    pBuf++;
                }
                strncpy(arr[args], head, pBuf - head);
                arr[args][pBuf-head] = '\0';
                if (head != pBuf)
                {
                    args++;
                }
            }
            while (*pBuf++);

            if (rcli_commands[i].func != NULL)
            {
                if (rcli_commands[i].func(args, (char**)arr) == -1)
                {
                    DEBUG_PRINT(("%s: Callback for '%s' command returned an error.\n", __func__, (char*)rcli_commands[i].argv[0]));
                }
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
        if (rcli_parse_cmd(bufStruct) == -1)
        {
        buf_debug(bufStruct);
            buf_clear(&bufStruct);
            break;
        }
        buf_debug(bufStruct);
        buf_clear(&bufStruct);
    }
    while(*p_tmp++);
    buf[0] = '\0';
    //unblock uart
}

void uart_rx(void)
{
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

        // BACKSPACE
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

        // LETTER
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

        // ENTER
        if (c == 13)
        {
            printf("\r\n");
            rcli_parse_buf(bufStruct.pBuf);

            //buf_debug(bufStruct);
            buf_clear(&bufStruct);
            
            sprintf(rcli_out_buf, "%s", RCLI_PROMPT_STR);
            RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
        }

        // TAB
        if (c == 9)
        {
            if( buf_get_count_params(bufStruct) == 1)
            {
                printf("\r\n");
               
                int i;
                int count_words = 0;
                int k = 0;
                char * cmd_str;
                for (i = 0; i < RCLI_CMD_SIZE - 1; i++)
                {
                    cmd_str = (char*)rcli_commands[i].argv[0];

                    char * pstr = strstr(cmd_str, bufStruct.pBuf);
                    if(pstr == rcli_commands[i].argv[0])
                    {
                        printf("  %s\r\n", cmd_str);
                        k = i;
                        count_words++;
                    }
                }
                sprintf(rcli_out_buf, "%s", RCLI_PROMPT_STR);
                RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
                if(count_words == 1)
                {
                    cmd_str = (char*)rcli_commands[k].argv[0]; 
                    buf_cpy_str(&bufStruct, cmd_str, strlen(cmd_str));
                    buf_add(&bufStruct, ' ', 0);
                    RcliTransferStr(bufStruct.pBuf, bufStruct.end);
                }
                else
                {
                    RcliTransferStr(bufStruct.pBuf, strlen(bufStruct.pBuf));
                }
            }
        }
    }
    system ("/bin/stty cooked");
    system("stty echo");
   // Call parse after timeout
}


int main(void)
{
    RcliInit();
    uart_rx();
    return 0; 
}
