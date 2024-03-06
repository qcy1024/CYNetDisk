#ifndef PROTOCOL_H
#define PROTOCOL_H



typedef unsigned int uint;

//通讯协议
struct PDU
{
    uint uiPDULen;      //协议数据单元大小(总的消息大小)
    uint uiMsgType;     //消息类型(数据是干嘛的)
    char caData[64];
    uint uiMsgLen;      //实际消息大小
    int caMsg[];        //实际消息部分，这个就是弹性数组，通过这东西来访问申请的动态空间

};

//产生一个PDU类型的结构体，变化部分是实际消息部分。
//这个函数就可以供应用程序调用，产生一个协议报文
PDU* mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
