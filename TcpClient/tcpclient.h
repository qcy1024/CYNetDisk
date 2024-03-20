#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>

#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
//登录成功时显示操作界面
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    //将配置文件的内容读到该类自身的成员中
    void loadConfig();

    static TcpClient& getInstance();
    QTcpSocket& getTcpSocket();

    QString loginName();    //返回m_strLoginName，即用户的登录名字
    QString curPath();      //返回用户当前所在的路径
    void setCurPath(QString strCurPath);      //设置新的当前所在路径

public slots:
    void showConnect();
    void recvMsg();

private slots:
    //void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;

    //存放配置文件中读到的服务器IP
    QString m_strIP;
    //16位无符号成员m_usPort存放配置文件中读到的服务器端口号
    quint16 m_usPort;

    //一个socket对象; m_tcpSocket是一个QTcpSocket对象
    QTcpSocket m_tcpSocket;

    //用户登录时输入的用户名
    QString m_strLoginName;

    //用户当前的路径
    QString m_strCurPath;

    QFile m_file;
};
#endif // TCPCLIENT_H
