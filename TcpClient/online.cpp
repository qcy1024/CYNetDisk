#include <string.h>
#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"  //!!

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if( pdu == NULL )
    {
        return ;
    }

    //uiMsgLen/32就是在线用户的数量
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];
    for( uint i=0; i<uiSize; ++i )
    {
        memcpy(caTmp,(char*)pdu->caMsg+i*32,32);
        ui->online_lw->addItem(caTmp);
    }
}

//“加好友”按钮触发函数
void Online::on_addFriend_pb_clicked()
{
    QListWidgetItem* pItem = ui->online_lw->currentItem();
    //PerName是要加的那个人
    QString strPerUsrName = pItem->text();
    QString strLoginName = TcpClient::getInstance().loginName();
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    //A要加B，前32字节是B，后32字节是A
    memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size()); //第三个参数应该是strPerUsrName.size()，试试32行不行？
    memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

