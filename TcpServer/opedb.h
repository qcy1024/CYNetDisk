#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>

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

    bool handleResigt(const char* name,const char* pwd);    //处理注册

signals:

public slots:

private:
    QSqlDatabase m_db;  //连接数据库



};

#endif // OPEDB_H
