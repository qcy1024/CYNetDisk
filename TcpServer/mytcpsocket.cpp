#include "mytcpsocket.h"
#include <QDebug>
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    //this发出的信号readyRead()由this的槽函数recvMsg()来处理。
    //readyRead()信号是当套接字收到数据时，由QTcpSocket对象发出的。
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));

    //客户端下线时，MyTcpSocket对象就会发出一个disconnected()信号。
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

//MyTcpSocket继承的是QTcpSocket
//当socket有数据过来了就会发出readyRead()信号，用它本身的槽函数recvMsg()来处理
void MyTcpSocket::recvMsg()
{
    qDebug() << this->bytesAvailable();
    //服务器端从套接字接收数据时，先读数据的总大小，用uiPUDLen保存
    uint uiPDULen = 0;
    //read的第一个参数要求为char*类型，含义是数据的存放位置，
    //read的第二个参数sizeof(uint)含义是接收的字节数。
    //sizeof(uint)为4个字节，这接收的4个字节的内容就是此次接收的消息的总大小。
    this->read((char*)&uiPDULen,sizeof(uint));

    //根据总的消息大小计算实际有效消息大小。
    uint uiMsgLen = uiPDULen - sizeof(PDU);

    //通过实际消息长度产生一个pdu来接收剩余数据
    PDU* pdu = mkPDU(uiMsgLen);
    //读入除uiPDULen字段之外的其他消息内容
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    qDebug() << pdu->uiMsgType << (char*)(pdu->caMsg);

    switch(pdu->uiMsgType)
    {
        //注册请求
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            qDebug() << "服务端处理出的用户名字为:" << caName << "密码为:" << caPwd << pdu->uiMsgType;
            bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);
            //对注册请求的回复无非就是注册成功或注册失败，用caData的64字节足够了。
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if( ret )
            {
                strcpy(respdu->caData,REGIST_OK);
            }
            else
            {
                strcpy(respdu->caData,REGIST_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        //登录请求
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if( ret )
            {
                strcpy(respdu->caData,LOGIN_OK);
                m_strName = caName;
            }
            else
            {
                strcpy(respdu->caData,LOGIN_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        //查看在线用户请求
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ret = OpeDB::getInstance().handleAllOnline();
            //每一个在线用户的用户名占32字节，一共有ret.size()个在线用户
            uint uiMsgLen = ret.size() * 32;
            PDU* respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for( int i=0; i<ret.size(); ++i )
            {
                //注意(char*)很关键，因为caMsg是int的指针，只有强制
                //转换成char*，才能正确计算得到地址
                memcpy((char*)(respdu->caMsg)+i*32
                       ,ret.at(i).toStdString().c_str()
                       ,ret.at(i).size());
                //qDebug() << "case为ALL_ONLINE_REQUEST时，回复的caMsg为:" << (char*)respdu->caMsg ;
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        //搜索好友请求
        case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
        {
            int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
            if( ret == -1 )
            {
                strcpy(respdu->caData,SEARCH_USR_NOT_EXIST);
            }
            else if( ret == 1 )
            {
                strcpy(respdu->caData,SEARCH_USR_ONLINE);
            }
            else if( ret == 0 )
            {
                strcpy(respdu->caData,SEARCH_USR_OFFLINE);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        //加好友请求
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            int ret = OpeDB::getInstance().handleAddFriend(caPerName,caName);

            PDU* respdu = NULL;
            //未知系统错误
            if( ret == -1 )
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,UNKNOW_ERROR);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            //已经是好友了
            else if( ret == 0 )
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,FRIEND_EXISTED);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            //用户在线
            else if( ret == 1 )
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
                memcpy(respdu->caData,caPerName,32);
                memcpy(respdu->caData+32,caName,32);
                //A要加B，PerName是B，
                //这里是因为要在套接字列表里面遍历，找到B的套接字，所以把这个任务交给了MyTcpServer
                MyTcpServer::getInstance().resend(caPerName,respdu);


            }
            //用户不在线
            else if( ret == 2 )
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            //用户不存在
            else if( ret == 3 )
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_NOTEXIST);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }

            break;
        }
        //客户端同意加好友
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
        {
            char name[32] = {'\0'};
            char pername[32] = {'\0'};
            memcpy(name,pdu->caData,32);
            memcpy(pername,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleAddFriendAgree(pername,name);
            PDU* respdu = mkPDU(0);
            if( ret )
            {
                //这里没写完
            }
            break;
        }
        //客户端拒绝加好友
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {

            break;
        }
        default:
            break;
    }//end of switch(pdu->uiMsgType)
    free(pdu);
    pdu = NULL;

}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    //发出信号offline
    emit offline(this);
}
