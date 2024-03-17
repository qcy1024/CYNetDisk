#include "mytcpsocket.h"
#include <QDebug>
#include "mytcpserver.h"
//QDir是QT里面封装的一个类，专门用于操作目录
#include <QDir>
#include <QFileInfoList>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    //this发出的信号readyRead()由this的槽函数recvMsg()来处理。
    //readyRead()信号是当套接字收到数据时，由QTcpSocket对象发出的。
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));

    //客户端下线时，MyTcpSocket对象就会发出一个disconnected()信号。
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
}

//返回这个登录用户的name(每一个用户在登录时都会创建一个不同的MyTcpSocket对象)
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
            //qDebug() << "服务端处理出的用户名字为:" << caName << "密码为:" << caPwd << pdu->uiMsgType;
            bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);
            //对注册请求的回复无非就是注册成功或注册失败，用caData的64字节足够了。
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if( ret )
            {
                strcpy(respdu->caData,REGIST_OK);

                QDir dir;
                //这里的./是服务器的运行目录，在本机上是build-TcpServer-Desktop_Qt_6_5_3_MinGW_64_bit-Debug这个目录
                qDebug() << "Create dir: " << dir.mkdir(QString("./%1").arg(caName));


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
                //这里没写完:客户端同意加好友了之后，把好友信息已经写入了数据库，返回值ret为
                //true说明操作成功，此时需要把加好友成功的信息返回给客户端，这里就没写。
            }
            break;
        }
        //客户端拒绝加好友
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {

            break;
        }
        //刷新好友请求
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);
            //有ret.size()个人，一个人占32个字节
            uint uiMsgLen = ret.size() * 32;
            PDU* respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for( int i=0; i<ret.size(); ++i )
            {
                memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str()
                       ,ret.at(i).size());
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        //删除好友请求
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char caSelfName[32] = {'\0'};
            char caFriendName[32] = {'\0'};
            strncpy(caSelfName,pdu->caData,32);
            strncpy(caFriendName,pdu->caData+32,32);
            OpeDB::getInstance().handleDelFriend(caSelfName,caFriendName);
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            strcpy(respdu->caData,DEL_FRIEND_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        //私聊请求
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            //注意私聊请求需要找到对方的socket，来转发信息，socket都保存在TcpServer里面。
            //我们可以给出要发送的pdu,以及要发给哪个人，根据函数
            //resend(const char* pername,PDU* pdu)来实现转发。
            char caPerName[32] = {'\0'};
            memcpy(caPerName,pdu->caData+32,32);
            //qDebug() << caPerName;
            //注意将这个pdu转发给客户端时，并没有对pdu做出任何改变，
            //因此，客户端收到的pdu->uiMsgType依然是REQUEST.
            MyTcpServer::getInstance().resend(caPerName,pdu);
        }
        //群聊请求
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        {
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            //handleFlushFriend()的返回值是caName的所有在线好友的一个QStringList
            QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName);
            for( int i=0; i<onlineFriend.size(); ++i )
            {
                QString tmp = onlineFriend.at(i);
                //注意这里的转发消息类型同样是没变，客户端收到的pdu类型依然是REQUEST
                MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu);
            }
            break;
        }
        //创建文件夹请求
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        {
            QDir dir;
            QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
            //qDebug() << "strCurPath = " << strCurPath;
            bool ret = dir.exists(strCurPath);
            PDU* respdu = NULL;
            //当前目录存在
            if( ret )
            {
                char caNewDir[32] = {'\0'};
                memcpy(caNewDir,pdu->caData+32,32);
                QString strNewPath = strCurPath + "/" + caNewDir;
                //qDebug() << strNewPath;
                ret = dir.exists(strNewPath);
                //qDebug() << "-->" << ret;
                //用户的当前目录下要创建的文件夹名字已存在
                if( ret )
                {
                    respdu = mkPDU(0);
                    strcpy(respdu->caData,DIR_NAME_EXIST);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                }
                //用户的当前目录下要创建的文件夹名字已存在
                else
                {
                    dir.mkdir(strNewPath);
                    respdu = mkPDU(0);
                    strcpy(respdu->caData,CREATE_DIR_OK);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                }
            }
            //当前目录不存在
            else
            {
                respdu = mkPDU(0);
                strcpy(respdu->caData,DIR_NO_EXIST);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }//end of case ENUM_MSG_TYPE_CREATE_DIR_REQUEST
        //刷新文件请求
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        {
            //用pCurPath保存客户端传过来的那个目录
            char* pCurPath = new char[pdu->uiMsgLen];
            memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
            QDir dir(pCurPath);
            QFileInfoList fileInfoList = dir.entryInfoList();
            //fileInfoList.size()即得到了该目录下有几个文件
            int iFileCount = fileInfoList.size();
            //文件数乘以一个文件的大小，即是整个pdu消息部分的大小。
            PDU* respdu = mkPDU(sizeof(FileInfo)*iFileCount);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            FileInfo* pFileInfo = NULL;
            QString strFileName;

            for( int i=0; i<iFileCount; ++i )
            {
                pFileInfo = (FileInfo*)(respdu->caMsg) + i;
                strFileName = fileInfoList[i].fileName();
                memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
                if( fileInfoList[i].isDir() )
                {
                    //0表示是一个目录
                    pFileInfo->iFileType = 0;
                }
                else if( fileInfoList[i].isFile() )
                {
                    //1表示是一个常规文件
                    pFileInfo->iFileType = 1;
                }
                // qDebug() << fileInfoList[i].fileName()
                //          << fileInfoList[i].size()
                //          << "文件夹:" << fileInfoList[i].isDir()
                //          << "常规文件:" << fileInfoList[i].isFile();
            }

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
        {
            //客户端发过来的是要删除的路径和文件名，我们首先将路径和文件名拼成一个完整的路径。
            char caName[64] = {'\0'};
            strcpy(caName,pdu->caData);
            //pdu->uiMsgLen是客户端传来的路径的长度
            char* pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caName);
            //qDebug() << strPath;
            //产生一个QFileInfo对象，用该对象来判断该路径是不是一个目录
            QFileInfo fileInfo(strPath);
            bool ret = false;
            //strPath是一个目录
            if( fileInfo.isDir() )
            {
                QDir dir;
                dir.setPath(strPath);
                //removeRecursively()是删除递归删除该目录
                ret = dir.removeRecursively();
            }
            //strPath是一个常规文件
            else if( fileInfo.isFile() )
            {
                //不删除
                ret = false;
            }
            PDU* respdu = NULL;
            //删除成功
            if( ret )
            {
                respdu = mkPDU(strlen(DEL_DIR_OK+1));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                memcpy(respdu->caData,DEL_DIR_OK,strlen(DEL_DIR_OK));
            }
            //删除失败
            else
            {
                respdu = mkPDU(strlen(DEL_DIR_OK+1));
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                memcpy(respdu->caData,DEL_DIR_FAILED,strlen(DEL_DIR_FAILED));
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
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
















