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
    //qDebug() << "handleLogin中，data为" << data;
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

//处理客户端下线
void OpeDB::handleOffline(const char *name)
{
    if( name == NULL )
    {
        return ;
    }
    QString data = QString("update usrInfo set online = 0 where name=\'%1\'").arg(name);
    //qDebug() << "handleOffline中，data为" << data;
    QSqlQuery query;
    query.exec(data);
}

//查询所有在线用户
QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from usrInfo where online=1");
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();

    while( query.next() )
    {
        result.append(query.value(0).toString());
    }

    // 调试信息
    // qDebug() << "handleAllOnline()中，result为:";
    // for( int i=0; i<result.size(); ++i )
    // {
    //     qDebug() << result.at(i);
    // }
    return result;
}

int OpeDB::handleSearchUsr(const char *name)
{
    if( name == NULL )
    {
        return -1;
    }

    QString data = QString("select online from usrInfo where name=\'%1\'").arg(name);

    QSqlQuery query;
    query.exec(data);

    if( query.next() )
    {
        int ret = query.value(0).toInt();
        if( ret == 1 )
        {
            return 1;
        }
        else if( ret == 0 )
        {
            return 0;
        }
    }
    //找的这个人不存在
    return -1;
}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if( pername == NULL || name == NULL )
    {
        return -1;
    }
    QString data = QString("select * from friend where (id=(select id from usrInfo where name = \'%1\') and friendId=(select id from usrInfo where name = \'%2\') ) "
            "or ( friendId = (select id from usrInfo where name = \'%3\') and id = (select id from usrInfo where name = \'%4\' ) )").arg(pername).arg(name).arg(name).arg(pername) ;
    //qDebug() << "在handleAddFriend中,data为：" << data;
    QSqlQuery query;
    query.exec(data);
    //他俩已经是好友了
    if(query.next())
    {
        //已经是好友了返回0
        return 0;
    }
    else
    {
        //查询好友是否在线
        QString data = QString("select online from usrInfo where name=\'%1\'").arg(pername);
        QSqlQuery query;
        query.exec(data);

        if( query.next() )
        {
            int ret = query.value(0).toInt();
            if( ret == 1 )
            {
                //在线返回1
                return 1;
            }
            else if( ret == 0 )
            {
                //不在线返回2
                return 0;
            }
        }
    }
    //查询的好友不存在
    return 3;
}

bool OpeDB::handleAddFriendAgree(const char *pername, const char *name)
{
    QString data = QString("select id from usrInfo where name=\'%1\' or name=\'%2\'").arg(pername).arg(name);
    QSqlQuery query;
    qDebug() << "data = " << data;
    query.exec(data);
    int id1 = 0,id2 = 0;
    if( query.next() )
        id1 = query.value(0).toInt();
    if( query.next() )
        id2 = query.value(0).toInt();
    qDebug() << "id1 = " << id1;
    qDebug() << "id2 = " << id2;
    data = QString("insert into friend (id,friendId) values(%1,%2)").arg(id1).arg(id2);
    qDebug() << "data = " << data;
    query.exec(data);
    return true;
}

//这里有bug，较大怀疑是数据库查询语句有问题
//就是数据库的问题。查询语句没问题，是数据库本身的问题。
QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if( name == NULL )
    {
        return strFriendList;
    }
    //要两次查询，分别是friend表中name的那个人作为id以及friendId。
    QString data = QString("select name from usrInfo where online = 1 and id = ("
                           "select id from friend where friendId=("
                           "select id from usrInfo where name=\'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(data);
    while( query.next() )
    {
        strFriendList.append(query.value(0).toString());
        //qDebug() << "handleFlushFriend中，一个好友是：" << query.value(0).toString();
    }

    data = QString("select name from usrInfo where online = 1 and id = ("
                           "select friendId from friend where id=("
                           "select id from usrInfo where name=\'%1\'))").arg(name);
    query.exec(data);
    while( query.next() )
    {
        strFriendList.append(query.value(0).toString());
        //qDebug() << "handleFlushFriend中，一个好友是：" << query.value(0).toString();
    }

    return strFriendList;
}

bool OpeDB::handleDelFriend(const char *name, const char *friendName)
{
    if( name == NULL || friendName == NULL )
    {
        return false;
    }
    QString data = QString("delete from friend where "
                           "id=( select id from usrInfo where name=\'%1\' )"
                           " and friendId=( select id from usrInfo where name=\'%2\' )")
                       .arg(name).arg(friendName);
    QSqlQuery query;
    query.exec(data);

    data = QString("delete from friend where "
                           "id=( select id from usrInfo where name=\'%1\' )"
                           " and friendId=( select id from usrInfo where name=\'%2\' )")
                       .arg(friendName).arg(name);
    query.exec(data);

    return true;
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
