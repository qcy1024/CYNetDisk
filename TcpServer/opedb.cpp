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

bool OpeDB::handleResigt(const char *name, const char *pwd)
{
    if( name == NULL || pwd == NULL )
    {
        return false;
    }
    QString data = QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);


    QSqlQuery query;
    //exec()函数的返回值是true或false，表示执行成功或者失败。
    return query.exec(data);

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
