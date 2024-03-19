#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_strEnterDir.clear();

    m_pTimer = new QTimer;

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

    connect(m_pRenamePB,SIGNAL(clicked(bool))
            ,this,SLOT(renameFile()));

    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex))
            ,this,SLOT(enterDir(QModelIndex)));

    connect(m_pReturnPB,SIGNAL(clicked(bool))
            ,this,SLOT(returnPre()));

    connect(m_pUploadPB,SIGNAL(clicked(bool))
            ,this,SLOT(uploadFile()));

    connect(m_pTimer,SIGNAL(timeout())
            ,this,SLOT(uploadFileData()));
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

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
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

//重命名文件
void Book::renameFile()
{
    //发送的请求为目录信息、文件名和新的文件名
    QString strCurPath = TcpClient::getInstance().curPath();
    QListWidgetItem* pItem = m_pBookListW->currentItem();
    //当前没有选中内容
    if( pItem == NULL )
    {
        QMessageBox::warning(this,"重命名文件","请选择要重命名的文件");
    }
    //当前选中了内容
    else
    {
        //获得所选中的内容的名字的字符串
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"重命名文件","请输入新的文件名");
        if( !strNewName.isEmpty() )
        {
            PDU* pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::warning(this,"重命名","文件名不能为空");
        }
    }



}

//双击文件夹进入的槽函数
void Book::enterDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString();
    m_strEnterDir = strDirName;
    //qDebug() << strDirName;

    QString strCurPath = TcpClient::getInstance().curPath();
    PDU* pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    //caData里面放用户双击的那个文件夹的名字
    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.size());
    //caMsg里面放用户当前所在的路径
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

//"返回"按钮槽函数
void Book::returnPre()
{
    /*注意返回的逻辑是：先把客户端这里保存的当前用户所在路径置为上一级，然后
    **调用刷新文件函数，刷新文件函数会根据TcpClient里面保存的用户当前所在*
    **路径，来向服务器请求该目录中的信息*/
    QString strCurPath = TcpClient::getInstance().curPath();
    QString strRootPath = "./" + TcpClient::getInstance().loginName();
    //用户已经在属于他的根目录下了
    if( strCurPath == strRootPath )
    {
        QMessageBox::warning(this,"返回","返回失败:已经在最开始的文件夹中");
    }
    else
    {   //"./aa/bb/cc" --> "./aa/bb"
        //lastIndexOf()返回一个整数，表示最后一个'/'在这个字符串中所在的位置
        int index = strCurPath.lastIndexOf('/');
        //从index开始删除，删除strCurPath.size() - index个。
        strCurPath.remove(index,strCurPath.size()-index);

        qDebug() << "return --> " << strCurPath;

        TcpClient::getInstance().setCurPath(strCurPath);
        clearEnterDir();
        flushFile();

    }

}

//"上传文件"按钮槽函数
void Book::uploadFile()
{
    QString strCurPath = TcpClient::getInstance().curPath();
    //这个函数会弹出一个打开文件的窗口，我们选择
    //一个文件之后，函数就会返回我们选择的文件的(绝对)路径
    m_strUploadFilePath = QFileDialog::getOpenFileName();

    qDebug() << m_strUploadFilePath;

    //我们只需要上传文件名字，需要在绝对路径中把名字提取出来
    //把文件名字、文件大小和用户当前所在路径通过pdu传到服务器
    if( !m_strUploadFilePath.isEmpty() )
    {
        //注意lastIndexOf是QString的类方法。
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
        qDebug() << strFileName;
        //定义一个QFile对象，把文件的绝对路径传进去
        QFile file(m_strUploadFilePath);
        //定义一个变量fileSize,保存文件的大小
        qint64 fileSize = file.size();
        //用户的当前所在路径放在uiMsg里面
        PDU* pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        //用户要上传的文件的文件名的文件大小放在caData里面。
        sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

        //定时器计时1000ms，到时间就会触发timeout信号。
        m_pTimer->start(1000);

    }
    else
    {
        QMessageBox::warning(this,"上传文件","文件不能为空");
    }


}

//定时器到时的槽函数(开始上传文件二进制数据)
void Book::uploadFileData()
{
    m_pTimer->stop();   //如果不手动停止计时器，计时器就会重新计时
    //m_strUploadFilePath是要上传的文件的绝对路径
    QFile file(m_strUploadFilePath);
    if( !file.open(QIODevice::ReadOnly) )
    {
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return ;
    }
    //每次读写数据多大性能最高？经过前人的多次测试，得到这个4096.
    char* pBuffer = new char[4096];
    qint64 ret = 0;
    while( true )
    {
        //读4096个字节到pBuffer所指示的位置处，read返回实际读到的字节数
        ret = file.read(pBuffer,4096);
        if( ret > 0 && ret <= 4096 )
        {
            //直接用socket二进制地传文件。
            TcpClient::getInstance().getTcpSocket().write((char*)pBuffer,ret);
        }
        else if( ret == 0)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this,"上传文件","上传文件失败:读文件失败");
            break;
        }
    }
    file.close();
    delete [] pBuffer;
    pBuffer = NULL;
}





























