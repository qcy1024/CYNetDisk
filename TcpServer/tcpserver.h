#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include "mytcpserver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpServer;
}
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();

    void loadConfig();

private:
    Ui::TcpServer *ui;
    //字符串类型
    QString m_strIP;
    //16位无符号成员m_usPort存放配置文件中读到的服务器端口号
    quint16 m_usPort;
};
#endif // TCPSERVER_H
