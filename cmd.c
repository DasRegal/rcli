#define CMD_LENGTH_CMD      16

{
    .cmd_name = "info",
    .min_params = 1,
    .max_params = 2,
};

cmd_list[] =
{
    cmd_info,
    NULL
};

void CmdInit()
{
    cmd_info->func = FuncInfo;
}

void CmdGetCmd()
{

}

void CmdParse (ctrlBuf_s *bufStruct)
{
    unsigned char i = 0;

    if (bufStruct->pBuf[0] == " " ||
        bufStruct->pBuf == "\t")
    {
    }

    while(bufStruct->pBuf[i] != " " ||
          bufStruct->pBuf[i] != "\t" || 
          i != bufStruct->end)
    {
        i++;
    }
}
