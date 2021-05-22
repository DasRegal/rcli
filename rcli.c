#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buf.h"

#define BUF_MAX 128
static unsigned char buf[BUF_MAX];

ctrlBuf_s bufStruct;

void HandleSymbol(char ch)
{

}

void IntUart()
{
    char ch;
    ch = getc(stdin);
    HandleSymbol(ch);
}

int main(void)
{
    BufInit(&bufStruct, buf, BUF_MAX);

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

/*    char ch;
    ch = 65;
    func(&ch);
    system ("/bin/stty raw");
    while(1)
    {
        ch = getc(stdin);
        if (ch == '\033')
        {
            if (!getc(stdin))
                break;
            if (getc(stdin) == 'D')
                printf("\e[%dD\e[K", 5);
            else
                printf("\e[%dD\e[K", 4);
        }
        else
            printf("\e[%dD%c", 1, ch);
    }
*/
    return 0;
}
