#include "tcpclient.h"
#include "ui_tcpclient.h"
//QByteArray提供了一种能够存储原始二进制数据的便捷方式
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QString>

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

    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));

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

void TcpClient::recvMsg()
{
    qDebug() << m_tcpSocket.bytesAvailable();
    //服务器端从套接字接收数据时，先读数据的总大小，用uiPUDLen保存
    uint uiPDULen = 0;
    //read的第一个参数要求为char*类型，含义是数据的存放位置，
    //read的第二个参数sizeof(uint)含义是接收的字节数。
    //sizeof(uint)为4个字节，这接收的4个字节的内容就是此次接收的消息的总大小。
    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));

    //根据总的消息大小计算实际有效消息大小。
    uint uiMsgLen = uiPDULen - sizeof(PDU);

    //通过实际消息长度产生一个pdu来接收剩余数据
    PDU* pdu = mkPDU(uiMsgLen);
    //读入除uiPDULen字段之外的其他消息内容
    m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    qDebug() << pdu->uiMsgType << (char*)(pdu->caMsg);

    switch(pdu->uiMsgType)
    {
        //注册回复
        case ENUM_MSG_TYPE_REGIST_RESPOND:
        {
            if( strcmp(pdu->caData,REGIST_OK) == 0 )
            {
                QMessageBox::information(this,"注册",REGIST_OK);
            }
            else if( strcmp(pdu->caData,REGIST_FAILED) == 0 )
            {
                QMessageBox::information(this,"注册",REGIST_FAILED);
            }
            break;
        }

        //登录回复
        case ENUM_MSG_TYPE_LOGIN_RESPOND:
        {
            if( strcmp(pdu->caData,LOGIN_OK) == 0 )
            {
                QMessageBox::information(this,"登录",LOGIN_OK);
            }
            else if( strcmp(pdu->caData,LOGIN_FAILED) == 0 )
            {
                QMessageBox::information(this,"登录",LOGIN_FAILED);
            }
            break;
        }
        default: break;
    }
    free(pdu);
    pdu = NULL;
}

#if 0
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
#endif

//登录按钮触发函数
void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if( !strName.isEmpty() && !strPwd.isEmpty() )
    {
        //用户名和密码信息放在协议数据单元的caData里面就行了，不用放在caMsg里面，所以消息实体的长度是0.
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        //caData的前32个字节存放name，后32个字节存放pwd
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败,用户名或密码为空");
    }
}

//注册按钮触发函数(click)
//客户端点击注册后，客户端程序获取用户输入的用户名及密码，将其通过套接字发往服务器
void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if( !strName.isEmpty() && !strPwd.isEmpty() )
    {
        //用户名和密码信息放在协议数据单元的caData里面就行了，不用放在caMsg里面，所以消息实体的长度是0.
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        //caData的前32个字节存放name，后32个字节存放pwd
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败,用户名或密码为空");
    }
}

//注销按钮点击操作触发
void TcpClient::on_cancel_pb_clicked()
{

}

