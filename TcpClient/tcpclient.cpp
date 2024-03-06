#include "tcpclient.h"
#include "ui_tcpclient.h"
//QByteArray提供了一种能够存储原始二进制数据的便捷方式
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();

    //直接在构造函数里连接tcp服务器

    //connect()将信号和信号处理函数关联起来。
    //四个参数分别是：信号发送方、发送的信号、信号接收方、接收方的信号处理函数
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));


    //连接服务器
    //connectToHost第一个参数是QHostAddress类型的服务器IP，第二个是整型的端口号
    //如果服务器成功连接，就会发出connected信号，我们在头文件里添加一个槽函数，捕获这个信号。
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);

    //在这里connectToHost了之后，m_tcpSocket就和一个具体的服务器端的套接字绑定了，之后
    //调用m_tcpSocket.write()就是写到那个绑定好了的服务端套接字。
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

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

//ui界面上的“发送”按钮的触发函数
void TcpClient::on_send_pb_clicked()
{
    //获取lineEdit(白色输入框)中的内容，存储到strMsg里面。
    QString strMsg = ui->lineEdit->text();
    //若框中内容不空，将其内容发给服务器
    if( !strMsg.isEmpty() )
    {
        //需要将消息转换为PDU类型，因为这是通讯协议
        PDU* pdu = mkPDU(strMsg.size()+1);
        //先随意赋予一个消息类型
        pdu->uiMsgType = 8888;
        //将要发送的数据--strMsg拷贝到协议数据单元pdu里面的caData那里去。
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
        //write()函数第一个参数要求是char*类型，第二个参数是消息总长度
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"信息发送","发送的信息不能为空");

    }
}

