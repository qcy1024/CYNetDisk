#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    //m_db是QSqlDatabase的一个对象
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    init();
}

//在析构函数中关闭数据库。
OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if( name == NULL || pwd == NULL )
    {
        return false;
    }
    QString data = QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    qDebug() << "handleRegist中，data为" << data;

    QSqlQuery query;
    //exec()函数的返回值是true或false，表示执行成功或者失败。
    return query.exec(data);

}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if( name == NULL || pwd == NULL )
    {
        return false;
    }
    QString data = QString("select * from usrInfo where name=\'%1\' and pwd = \'%2\' and online = 0").arg(name).arg(pwd);
    qDebug() << "handleLogin中，data为" << data;
    QSqlQuery query;
    query.exec(data);
    //next()获取查询结果中的下一条数据，将数据放到query对象中去。
    //如果成功获取，next()返回真，如果获取失败，next()返回假。
    //这里的登录逻辑中，在该查询条件下，如果next()能获取到一条数据，说明可以登录成功。并且由于设计
    //数据库的时候name是unique的，所以不会有多条相同记录。
    bool loginOk = query.next();
    if( loginOk )
    {
        QString data = QString("update usrInfo set online=1 where name=\'%1\' and pwd = \'%2\'").arg(name).arg(pwd);
        QSqlQuery query;
        query.exec(data);
    }
    else
    {

    }
    return loginOk;

}

void OpeDB::handleOffline(const char *name)
{
    if( name == NULL )
    {
        return ;
    }
    QString data = QString("update usrInfo set online = 0 where name=\'%1\'").arg(name);
    qDebug() << "handleOffline中，data为" << data;
    QSqlQuery query;
    query.exec(data);
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    //用QSqlDatabase的对象操作数据库: 1.设置主机名setHostName，2.设置数据库名setDatabaseName
    //3.打开数据库open()，4.操作数据库
    //设置主机名，是本机就写localhost，
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\QT\\QTProjects\\TcpServer\\cloud.db");
    if( m_db.open() )
    {
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while( query.next() )
        {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");

    }
}
