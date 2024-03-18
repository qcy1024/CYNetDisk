/*这个book类就用做文件操作的界面*/
#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU* pdu);
    void clearEnterDir();
    QString enterDir();
signals:

public slots:
    void createDir();   //创建目录按钮
    void flushFile();   //刷新文件按钮
    void delDir();      //删除文件夹按钮
    void renameFile();
    //槽函数的形参要与信号的形参保持一致
    void enterDir(const QModelIndex& index);
    void returnPre();       //返回按钮
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
};

#endif // BOOK_H
