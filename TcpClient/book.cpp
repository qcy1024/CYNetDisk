#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_pBookListW = new QListWidget;


    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件");
    m_pFlushPB = new QPushButton("刷新文件");

    QVBoxLayout* pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushPB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownloadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("共享文件");

    QVBoxLayout* pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownloadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);

    QHBoxLayout* pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool))
            ,this,SLOT(createDir()));

    connect(m_pFlushPB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFile()));

    connect(m_pDelDirPB,SIGNAL(clicked(bool))
            ,this,SLOT(delDir()));

}

void Book::updateFileList(const PDU *pdu)
{
    if( pdu == NULL )
    {
        return ;
    }
    QListWidgetItem* pItemTmp = NULL;
    int row = m_pBookListW->count();
    while( m_pBookListW->count() > 0 )
    {
        pItemTmp = m_pBookListW->item(row-1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row = row - 1;
    }
    FileInfo* pFileInfo = NULL;
    //iCount是文件数量
    int iCount = pdu->uiMsgLen / sizeof(FileInfo);
    for( int i=0; i<iCount; ++i )
    {
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;
        //qDebug() << pFileInfo->caFileName << pFileInfo->iFileType;
        //循环中每一个文件或文件夹都有一个QListWidgetItem来代表
        QListWidgetItem* pItem = new QListWidgetItem;
        if( pFileInfo->iFileType == 0 )
        {
            //设置图标为文件夹图标
            pItem->setIcon(QIcon(QPixmap(":/map/DirIcon.png")));
        }
        else if( pFileInfo->iFileType == 1 )
        {
            //设置图标为文件图标
            pItem->setIcon(QIcon(QPixmap(":/map/FileIcon.jpg")));
        }
        //设置文本为caFileName
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
    }

}

//点击"新建文件夹"按钮的槽函数
void Book::createDir()
{
    //这个函数返回一个字符串，就是你输入的文件夹的名字
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新文件夹名字");
    if( !strNewDir.isEmpty() )
    {
        if( strNewDir.size() > 32 )
        {
            QMessageBox::warning(this,"新建文件夹","新文件夹名字不能超过32字符");
        }
        //创建文件夹请求要把3个信息发送给服务器：
        //1.客户端的用户名   2.客户端当前所在路径   3.新文件夹名字
        //我们把客户的名字和新文件夹名字放在caData里，客户端当前所在路径放在caMsg里。
        else
        {
            QString strName = TcpClient::getInstance().loginName();
            QString strCurPath = TcpClient::getInstance().curPath();
            PDU* pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
            strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
            memcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","新文件夹名字不能为空");
    }

}

//"刷新文件"按钮的触发函数
void Book::flushFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    //将路径放在caMsg里面
    PDU* pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

//按钮"删除文件夹"的触发函数
void Book::delDir()
{
    //获得当前所在目录
    QString strCurPath = TcpClient::getInstance().curPath();
    //获得界面上当前选中的内容
    QListWidgetItem* pItem = m_pBookListW->currentItem();
    //当前没有选中内容
    if( pItem == NULL )
    {
        QMessageBox::warning(this,"删除文件","请选择要删除的文件");
    }
    //当前选中了内容
    else
    {
        //获得所选中的内容的名字的字符串
        QString strDelName = pItem->text();
        PDU* pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        //要删除的名字放在caData里面
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        //要删除的路径放在caMsg里面
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}





























