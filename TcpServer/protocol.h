#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed: name existed"
#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed"
#define SEARCH_USR_NOT_EXIST "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"
//未知系统错误
#define UNKNOW_ERROR "unknow error"
#define FRIEND_EXISTED "friend existed"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NOTEXIST "usr not exist"

//消息类型的枚举
enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,

    ENUM_MSG_TYPE_REGIST_REQUEST,       //注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,       //注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,       //登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,       //登录回复

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,       //查看在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,       //查看在线用户回复

    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,       //搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,       //搜索用户回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,       //加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,       //加好友回复

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,       //同意加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,       //拒绝加好友

    ENUM_MSG_TYPE_MAX = 0x00ffffff,



};

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
