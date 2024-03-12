#ifndef OPEDB_H
#define OPEDB_H

/*所有对数据库的操作都封装到该类OpeDB里面*/

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);

    //外界想要得到单例类OpeDB的对象时，可以直接通过作用域运算符
    //获取OpeDB类的实例；因为是static的，所以每次获取的都是同一个对象。
    static OpeDB& getInstance();

    void init();
    ~OpeDB();

    bool handleRegist(const char* name,const char* pwd);    //处理注册
    bool handleLogin(const char* name,const char* pwd);    //处理登录
    void handleOffline(const char* name);                  //处理客户端下线
    QStringList handleAllOnline();  //处理查看在线用户，将所有在线用户放在一个QStringList里面返回(字符串列表)
    int handleSearchUsr(const char* name);  //查找用户
    //返回值是int因为要处理多于2的情况，如用户已经是好友、用户不在线、用户回复同意、用户不同意等。
    int handleAddFriend(const char* pername,const char* name);
    //客户端同意添加好友后服务器的处理
    bool handleAddFriendAgree(const char* pername,const char* name);
signals:

public slots:

private:
    QSqlDatabase m_db;  //连接数据库



};

#endif // OPEDB_H
