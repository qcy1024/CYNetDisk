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
#define UNKNOW_ERROR "unknow error"     ////未知系统错误
#define FRIEND_EXISTED "friend existed"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NOTEXIST "usr not exist"
#define DEL_FRIEND_OK "delete friend ok"
#define DIR_NO_EXIST "dir not exist"
#define DIR_NAME_EXIST "file name exist"
#define CREATE_DIR_OK "create dir ok"
#define DEL_DIR_OK "delete dir ok"
#define DEL_DIR_FAILED "delete dir failed"
#define RENAME_FILE_OK "rename file ok"
#define RENAME_FILE_FAILED "rename file failed"
#define ENTER_DIR_FAILED "enter dir failed"
#define ENTER_DIR_OK "enter dir ok"
#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILED "upload file failed"
#define DEL_FILE_OK "delete file ok"
#define DEL_FILE_FAILED "delete file failed"

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

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,       //刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,       //刷新好友回复

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,       //删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,       //删除好友回复

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,       //私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,       //私聊回复

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,       //群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,       //群聊回复

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,       //创建目录请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,       //创建目录回复

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,       //查看(刷新)文件请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,       //查看(刷新)文件回复

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,       //删除目录请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,       //删除目录回复

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,       //重命名文件请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,       //重命名文件回复

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,    //进入文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,    //进入文件夹回复

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,  //上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,  //上传文件回复

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,     //删除常规文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,     //删除常规文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,  //下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,  //下载文件回复

    ENUM_MSG_TYPE_MAX = 0x00ffffff,



};

struct FileInfo
{
    char caFileName[32];    //文件名
    int iFileType;  //文件类型
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
