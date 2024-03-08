#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"

//MyTcpSocket继承了QTcpSocket,可以直接用QTcpSocket提供的功能。
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);

    QString getName();

//信号
signals:
    void offline(MyTcpSocket* mysocket);

//槽函数
public slots:
    void recvMsg();
    void clientOffline();   //处理客户端下线

private:
    //保存登录的用户的name(这个项目是不是只支持单连接？)
    QString m_strName;
};

#endif // MYTCPSOCKET_H
