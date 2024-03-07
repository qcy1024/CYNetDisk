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

//槽函数
public slots:
    void recvMsg();
};

#endif // MYTCPSOCKET_H
