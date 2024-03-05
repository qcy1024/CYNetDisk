#include "tcpclient.h"
#include "ui_tcpclient.h"
//QByteArray提供了一种能够存储原始二进制数据的便捷方式
#include <QByteArray>
#include <QDebug>

TcpClient::TcpClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
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
