#include "protocol.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>




//接受实际消息的长度作为参数
//返回一个PDU*指针
PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen = sizeof(PDU) + uiMsgLen;
    PDU* pdu = (PDU*)malloc(uiPDULen);
    memset(pdu,0,uiPDULen);
    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
