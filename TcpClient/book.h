/*这个book类就用做文件操作的界面*/
#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU* pdu);
    void clearEnterDir();
    QString enterDir();
    void setDownloadStatus(bool status);
    bool getDownloadStatus();
    QString getSaveFilePath();



    //为了省事就写成公有的了
    qint64 m_iTotal;    //要下载的文件总大小
    qint64 m_iRecved;   //已经接收了的大小


signals:

public slots:
    void createDir();   //创建目录按钮
    void flushFile();   //刷新文件按钮
    void delDir();      //删除文件夹按钮
    void renameFile();
    //槽函数的形参要与信号的形参保持一致
    void enterDir(const QModelIndex& index);
    void returnPre();       //返回按钮
    void uploadFile();      //上传文件按钮

    void uploadFileData();  //发送文件数据(二进制)
    void delRegFile();      //删除常规文件
    void downloadFile();    //下载文件

private:
    QListWidget* m_pBookListW;  //显示文件名字的列表

    QPushButton* m_pReturnPB;
    QPushButton* m_pCreateDirPB;    //创建目录按钮
    QPushButton* m_pDelDirPB;       //删除文件夹按钮
    QPushButton* m_pRenamePB;
    QPushButton* m_pFlushPB;        //刷新(查看)文件按钮
    QPushButton* m_pUploadPB;
    QPushButton* m_pDownloadPB;
    QPushButton* m_pDelFilePB;
    QPushButton* m_pShareFilePB;

    //该变量在用户双击进入某一目录时被置值
    QString m_strEnterDir;      //用户双击某一目录时，要进入的目录路径

    QString m_strUploadFilePath;    //上传文件时，用来保存用户选择的路径

    QTimer* m_pTimer;

    QString m_strSaveFilePath;  //下载文件时保存文件的路径,该变量在用户下载文件选完保存路径时被赋值
    bool m_bDownload;           //是否处于下载状态



};

#endif // BOOK_H
