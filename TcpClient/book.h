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
signals:

public slots:
    void createDir();
    void flushFile();

private:
    QListWidget* m_pBookListW;  //显示文件名字的列表

    QPushButton* m_pReturnPB;
    QPushButton* m_pCreateDirPB;    //创建目录按钮
    QPushButton* m_pDelDirPB;
    QPushButton* m_pRenamePB;
    QPushButton* m_pFlushPB;        //刷新(查看)文件按钮
    QPushButton* m_pUploadPB;
    QPushButton* m_pDownloadPB;
    QPushButton* m_pDelFilePB;
    QPushButton* m_pShareFilePB;
};

#endif // BOOK_H
