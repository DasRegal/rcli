#include <stdio.h>
#include <string.h>

#include "buf.h"

// static void BufDebug(ctrlBuf_s *bufStruct);

unsigned char BufInit(ctrlBuf_s *bufStruct, unsigned char *pBuf, unsigned char max_size)
{
    unsigned char res   = 0;

    if (bufStruct == NULL)
    {
        return 1;
    }

    bufStruct->cur_pos  = 1;
    bufStruct->end      = 0;
    bufStruct->is_clear = 1;
    bufStruct->pBuf     = pBuf;
    bufStruct->max_size = max_size;

    return res;
}

unsigned char BufAdd(ctrlBuf_s *bufStruct, unsigned char symbol, unsigned char pos)
{
    unsigned char res = 0;

    if (bufStruct == NULL)
    {
        return 1;
    }

    if (bufStruct->end >= bufStruct->max_size)
    {
        return 1;
    }

    unsigned char end    = bufStruct->end;
    unsigned char *pBuf  = bufStruct->pBuf;
    unsigned char *pDest = NULL;
    unsigned char *pSrc  = NULL;
    unsigned char len    = 0;

    if (pos > end + 1)
    {
        pos = end + 1;
    }

    if (pos == 0)
    {
        pos = bufStruct->cur_pos;
    }

    if (pos - 1 >= 0)
    {
        len = pos - 1;
    }

    if (len > 0)
    {
        pSrc  = pBuf + sizeof(unsigned char) * (end - pos + 1);
        pDest = pSrc + sizeof(unsigned char);
        memcpy(pDest, pSrc, len);
    }
    pBuf[end - len] = symbol;

    bufStruct->end     += 1;
    bufStruct->is_clear = 0;
    bufStruct->cur_pos  = pos;

    return res;
}

unsigned char BufMoveCur(ctrlBuf_s *bufStruct, dirBuf_e dir)
{
    unsigned char res = 0;
    unsigned char pos;

    if (bufStruct == NULL)
    {
        return 1;
    }

    pos = bufStruct->cur_pos;

    switch(dir)
    {
        case BUF_CUR_LEFT:
        {
            if (pos - 1 < bufStruct->end)
            {
                pos += 1;
            }
            break;
        }

        case BUF_CUR_RIGHT:
        {
            if (pos > 1)
            {
                pos -= 1;
            }
            break;
        }

        case BUF_CUR_HOME:
        {
            pos = bufStruct->end + 1;
            break;
        }

        case BUF_CUR_END:
        {
            pos = 1;
            break;
        }

        default:
        {
            return 1;
        }
    }

    bufStruct->cur_pos = pos;
    return res;
}

unsigned char BufDel(ctrlBuf_s *bufStruct)
{
    unsigned char pos;
    unsigned char end;
    unsigned char *pBuf;
    unsigned char *pDest = NULL;
    unsigned char *pSrc  = NULL;
    unsigned char len    = 0;
    unsigned char res    = 0;

    if (bufStruct == NULL)
    {
        return 1;
    }

    pos  = bufStruct->cur_pos;
    end  = bufStruct->end;
    pBuf = bufStruct->pBuf;

    if (pos <= 1)
    {
        return res;
    }

    if (pos > 2)
    {
        len   = pos - 2;
        pDest = pBuf + sizeof(unsigned char) * (end - pos + 1);
        pSrc  = pDest + sizeof(unsigned char);
        memcpy(pDest, pSrc, len);
    }

    bufStruct->pBuf[end - 1] = 0;
    bufStruct->end -= 1;
    bufStruct->cur_pos -= 1;
    
    if (bufStruct->end == 0)
    {
        bufStruct->is_clear = 1;
    }

    return res;
}

unsigned char BufClear(ctrlBuf_s *bufStruct)
{
    unsigned char res = 0;

    if (bufStruct == NULL)
    {
        return 1;
    }

    // memset is evil. Param size in bytes.
    memset(bufStruct->pBuf, '\0', bufStruct->max_size);

    bufStruct->is_clear = 1;
    bufStruct->cur_pos  = 1;
    bufStruct->end      = 0;

    return res;
}

void BufDebug(ctrlBuf_s bufStruct)
{
    printf("Debug:\n");
    printf("\t cur_pos: %d\n", bufStruct.cur_pos);
    printf("\t end: %d\n", bufStruct.end);
    printf("\t is_clear: %d\n", bufStruct.is_clear);
    printf("\t pBuf: %p\n", bufStruct.pBuf);
    printf("\t max_size: %d\n", bufStruct.max_size);
    printf("\t buf=\"%s\"\n", bufStruct.pBuf);
}
