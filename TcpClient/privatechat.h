/*该文件为私聊功能的界面ui*/
#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = nullptr);
    ~PrivateChat();

    static PrivateChat& getInstance();

    void setChatName(QString strName);
    //客户端收到消息时，根据收到的pdu将消息显示在聊天界面中
    void updateMsg(const PDU* pdu);


private slots:
    void on_sendMsg_pb_clicked();

private:
    Ui::PrivateChat *ui;

    QString m_strChatName;
    QString m_strLoginName; //登录的用户名
};

#endif // PRIVATECHAT_H
