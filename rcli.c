#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buf.h"

#define BUF_MAX 128

static unsigned char buf[BUF_MAX];
static unsigned char rcli_out_buf[BUF_MAX];

static ctrlBuf_s bufStruct;

#define RCLI_PROMPT_STR   "> "
#define RCLI_PROMPT_SHIFT 2    /* strlen of RCLI_PROMPT_STR */

unsigned char RcliTransferChar(const char ch)
{
    printf("%c", ch);
    return 0;
}

void RcliReceiveChar(char ch)
{
}

static unsigned char RcliTransferStr(unsigned char * const pBuf, unsigned char len)
{
    unsigned char *buf = pBuf;

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
    BufInit(&bufStruct, buf, BUF_MAX);
    sprintf(rcli_out_buf, "Welcome (RCli)\n");
    RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
    
    sprintf(rcli_out_buf, "%s", RCLI_PROMPT_STR);
    RcliTransferStr(rcli_out_buf, strlen(rcli_out_buf));
}

int main(void)
{
    RcliInit();

    BufAdd(&bufStruct, 'a', 0);
    BufAdd(&bufStruct, 'b', 0);
    BufAdd(&bufStruct, 'c', 0);
    BufAdd(&bufStruct, 'd', 0);
    BufAdd(&bufStruct, 'e', 0);
    BufMoveCur(&bufStruct, BUF_CUR_LEFT);
    BufMoveCur(&bufStruct, BUF_CUR_LEFT);
    BufMoveCur(&bufStruct, BUF_CUR_LEFT);
    BufMoveCur(&bufStruct, BUF_CUR_LEFT);
    BufAdd(&bufStruct, '3', 0);
    BufMoveCur(&bufStruct, BUF_CUR_RIGHT);
    BufMoveCur(&bufStruct, BUF_CUR_RIGHT);
    BufMoveCur(&bufStruct, BUF_CUR_RIGHT);
    BufAdd(&bufStruct, '4', 0);
    BufMoveCur(&bufStruct, BUF_CUR_HOME);
    BufAdd(&bufStruct, 'H', 0);
    BufMoveCur(&bufStruct, BUF_CUR_END);
    BufAdd(&bufStruct, 'E', 0);

    printf("%s\n", buf);

    BufMoveCur(&bufStruct, BUF_CUR_HOME);
    BufDel(&bufStruct);
    printf("%s\n", buf);

    printf("Clear buf...\n");
    BufClear(&bufStruct);
    printf("%s\nDone\n", buf);
    BufDebug(bufStruct);

    BufAdd(&bufStruct, 'a', 0);
    BufAdd(&bufStruct, 'b', 0);
    BufAdd(&bufStruct, 'c', 0);
    printf("%s\n", buf);

    // char ch;
    // while(1)
    // {
    //     ch = getc(stdin);
    //     // HandleSymbol(ch);
    // }

    char ch;
//    ch = 65;
//    func(&ch);
//    system ("/bin/stty raw");
    while(1)
    {
        ch = getc(stdin);
        if (ch == '\033')
        {
            printf("here 0\n");
            if (!getc(stdin))
                break;
            if (getc(stdin) == 'D')
            {
                printf("\e[%dA\e[K", 2);
            }
            else
                printf("\e[%dD\e[K", 4);
        }
        else
            printf("\e[%dD%c", 1, ch);
    }
    return 0;
}
