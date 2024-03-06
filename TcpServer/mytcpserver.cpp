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

}
