#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer() {}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

//只要有客户端连接到服务器，incomingConnection()就会自动调用。参数socketDescriptor就是
//发起连接那一方的套接字描述符
void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "新的连接已连接！";
    //这里是要在新的连接到来时，使用自己的套接字描述方式MyTcpSocket来保存新的套接字描述符。
    MyTcpSocket* pTcpSocket = new MyTcpSocket;

    //设置套接字描述符为发起连接的那一方的套接字描述符
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));
}

//槽函数和信号的参数要一样(这个槽函数是要接收mytcpsocket的offline信号)
void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::Iterator iter = m_tcpSocketList.begin();
    for( ; iter!=m_tcpSocketList.end(); ++iter )
    {
        if( mysocket == *iter )
        {
            delete *iter;
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }
    for( int i=0; i<m_tcpSocketList.size(); ++i )
    {
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}
