#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"

using namespace std;
class connection_pool
{
public:
    MYSQL* getConnection();    //获取数据库连接
    bool releaseConnection(MYSQL* conn);  //释放连接
    int getFreeConn();    //获取连接
    void destoryPool();     //销毁所有连接

    //单例模式
    static connection_pool* getInstance();

    //初始化
    void init(string url,string user,string password,string databaseName,int port,int maxConn,int close_log);
private:
    connection_pool();
    ~connection_pool();

    int m_maxConn;          //最大连接数
    int m_curConn;          //当前已使用的连接数
    int m_freeConn;         //当前空闲的连接数
    locker lock;
    list<MYSQL*> connList;  //连接池
    sem reserve;
public:
    string m_url;   //主机地址
    int m_port;  //数据库端口地址
    string m_user;  //登录数据库用户名
    string m_password;//登录数据库密码
    string m_databaseName;//使用数据库名
    int m_close_log;//日志开关
};

class connectionRAII
{
public:
    connectionRAII(MYSQL **conn,connection_pool*connPool);
    ~connectionRAII();

private:
    MYSQL* m_conn;
    connection_pool* m_poolRAII;
};


#endif