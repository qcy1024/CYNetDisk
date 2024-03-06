#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

//将MyTcpServer设计成单例类
//MyTcpServer继承了QTcpServer,QTcpServer是用于实现TCP服务器的。
class MyTcpServer : public QTcpServer
{
    //使类支持信号槽
    Q_OBJECT

public:
    MyTcpServer();

    static MyTcpServer &getInstance();

    void incomingConnection(qintptr socketDescriptor);

private:
    QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
