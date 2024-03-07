#include "tcpserver.h"
#include "ui_tcpserver.h"

#include <QByteArray>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QFile>


TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    //MyTcpServer继承了QTcpServer,QTcpServer是用于实现TCP服务器的。
    //listen的参数:QHostAddress类型的IP地址、整型的端口号
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);

    //注意是MyTcpServer的一个对象在监听，只要有客户端连接到服务器，
    //incomingConnection()就会自动调用。
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if( file.open(QIODevice::ReadOnly) )
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        file.close();
        strData.replace("\r\n"," ");

        //QStringList是存储字符串的容器类，它是QString的列表。
        QStringList strList = strData.split(" ");

        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "ip:" << m_strIP << "port:" << m_usPort;
    }
}
