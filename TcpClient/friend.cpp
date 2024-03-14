#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>
#include "privatechat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit;  //显示信息
    m_pFriendListWidget = new QListWidget;   //显示好友列表
    m_pInputMsgLE = new QLineEdit;     //信息输入框

    m_pDelFriendPB = new QPushButton("删除好友");    //删除好友按钮
    m_pFlushFriendPB = new QPushButton("刷新好友");  //刷新在线好友按钮
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");    //查看在线的用户
    m_pSearchUsrPB = new QPushButton("查找用户");    //查找用户按钮
    m_pMsgSendPB = new QPushButton("信息发送");      //发送消息按钮
    m_pPrivateChatPB = new QPushButton("私聊");  //私聊按钮

    //以下几项放在一个垂直布局
    QVBoxLayout* pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    //以下几项放在一个水平布局
    QHBoxLayout* pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout* pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout* pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    //online窗口一开始不显示，当我们点击“显示在线用户”时它才显示出来。
    m_pOnline->hide();

    setLayout(pMain);

    //"显示在线用户"按钮被点击时的信号槽
    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),
            this,SLOT(showOnline()));

    connect(m_pSearchUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(searchUsr()));

    connect(m_pFlushFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFriend()));

    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));

    connect(m_pPrivateChatPB,SIGNAL(clicked(bool))
            ,this,SLOT(privateChat()));
}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if( pdu == NULL )
    {
        return ;
    }
    m_pOnline->showUsr(pdu);

    return;
}

void Friend::updateFriendList(PDU *pdu)
{
    if( pdu == NULL )
    {
        return ;
    }
    uint uiSize = pdu->uiMsgLen / 32;
    char caName[32] = {'\0'};
    for( uint i=0; i<uiSize; i++ )
    {
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);;
        m_pFriendListWidget->addItem(caName);
    }
}

//点击"查看在线用户"的触发函数
void Friend::showOnline()
{
    if( m_pOnline->isHidden() )
    {
        m_pOnline->show();
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        m_pOnline->hide();
    }
}

//点击"查找用户"的触发函数
void Friend::searchUsr()
{
    //getText()的返回值就是用户在输入框里输入的内容
    m_strSearchName = QInputDialog::getText(this,"搜索","用户名");
    if( !m_strSearchName.isEmpty() )
    {
        //qDebug() << name;
        //要搜索的用户名放在caData里面就行了。
        PDU* pdu = mkPDU(0);
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

//"刷新好友"按钮的触发函数
void Friend::flushFriend()
{
    //点击“刷新好友”时，要把自己的用户名发给服务器，这样服务器才能知道你有哪些好友
    QString strName = TcpClient::getInstance().loginName();
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()
{
    QString strFriendName = m_pFriendListWidget->currentItem()->text();
    //qDebug() << "delFriend中，strName:" << strName;
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    QString strSelfName = TcpClient::getInstance().loginName();

    memcpy(pdu->caData,strSelfName.toStdString().c_str(),strSelfName.size());
    memcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

//点击"私聊"按钮的槽函数
void Friend::privateChat()
{
    if( m_pFriendListWidget->currentItem() != NULL )
    {
        QString strChatName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if( PrivateChat::getInstance().isHidden() )
        {
            PrivateChat::getInstance().show();
        }
    }
    else
    {
        QMessageBox::warning(this,"私聊","请选择私聊的对象");
    }
}

























