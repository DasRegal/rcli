#ifndef BUF_H
#define BUF_H

typedef struct
{
    unsigned char cur_pos;
    unsigned char end;
    unsigned char is_clear;
    unsigned char* pBuf;
    unsigned char max_size;
} ctrlBuf_s;

typedef enum
{
    BUF_CUR_LEFT,
    BUF_CUR_RIGHT,
    BUF_CUR_HOME,
    BUF_CUR_END,
} dirBuf_e;

unsigned char BufInit(ctrlBuf_s *bufStruct, unsigned char *pBuf, unsigned char max_size);
unsigned char BufAdd(ctrlBuf_s *bufStruct, unsigned char symbol, unsigned char pos);
unsigned char BufMoveCur(ctrlBuf_s *bufStruct, dirBuf_e dir);
unsigned char BufDel(ctrlBuf_s *bufStruct);
unsigned char BufClear(ctrlBuf_s *bufStruct);
void BufDebug(ctrlBuf_s bufStruct);

#endif