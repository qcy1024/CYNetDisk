#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

//“显示在线用户”按钮点击，会弹出在线好友窗口，online.h
#include "online.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUsr(PDU* pdu);
    void updateFriendList(PDU* pdu);

    QString m_strSearchName;    //临时的名字，用于查找时的客户端提示

signals:

public slots:
    void showOnline();
    void searchUsr();
    void flushFriend();     //点击"刷新好友"按钮的触发函数
    void delFriend();       //点击"删除好友"按钮的触发函数
    void privateChat();     //点击"私聊"的触发函数

private:
    QTextEdit* m_pShowMsgTE;  //显示信息
    QListWidget* m_pFriendListWidget;   //显示好友列表
    QLineEdit* m_pInputMsgLE;     //信息输入框

    QPushButton* m_pDelFriendPB;    //删除好友按钮
    QPushButton* m_pFlushFriendPB;  //刷新在线好友按钮
    QPushButton* m_pShowOnlineUsrPB;    //查看在线的用户
    QPushButton* m_pSearchUsrPB;    //查找用户按钮
    QPushButton* m_pMsgSendPB;      //发送消息按钮
    QPushButton* m_pPrivateChatPB;  //私聊按钮

    Online* m_pOnline;



};

#endif // FRIEND_H
