#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QDir>
#include "protocol.h"
#include "opedb.h"
#include <QFile>

//MyTcpSocket继承了QTcpSocket,可以直接用QTcpSocket提供的功能。
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);

    QString getName();  //返回这个登录用户的name

//信号
signals:
    void offline(MyTcpSocket* mysocket);

//槽函数
public slots:
    void recvMsg();
    void clientOffline();   //处理客户端下线

private:
    //保存登录的用户的name(这个项目是不是只支持单连接？)
    //不是的。因为每一个用户在登录时都会创建一个不同的myTcpSocket对象，每个
    //不同的myTcpSocket对象都有着自己的m_strName成员。
    QString m_strName;

    QFile m_file;
    qint64 m_iTotal;    //文件总大小
    qint64 m_iRecved;   //文件已经接收了的大小
    bool m_bUpload;    //判断是否处于上传文件状态,在构造函数中被置为false
};

#endif // MYTCPSOCKET_H
