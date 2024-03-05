#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>

#include <QFile>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QMainWindow
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    void loadConfig();
private:
    Ui::TcpClient *ui;
    //存放配置文件中读到的服务器IP
    QString m_strIP;
    //16位无符号成员m_usPort存放配置文件中读到的服务器端口号
    quint16 m_usPort;

    //一个socket对象

};
#endif // TCPCLIENT_H
