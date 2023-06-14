#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_connection_pool.h"

using namespace std;

connection_pool::connection_pool()
{
    m_curConn = 0;
    m_freeConn = 0;
}

connection_pool::~connection_pool()
{
    destoryPool();
}

connection_pool* connection_pool::getInstance()
{
    static connection_pool connPool;
    return &connPool;
}

//构造初始化
void connection_pool::init(string url,string user,string password,string databaseName,int port,int maxConn,int close_log)
{
    m_url = url;
    m_user = user;
    m_password = password;
    m_port = port;
    m_databaseName = databaseName;
    m_close_log = close_log;
    m_maxConn = maxConn;

    for(int i = 0; i < m_maxConn;i++)
    {

        LOG_INFO("%s%d","开始连接数据库：",i + 1);
        MYSQL *con = NULL;
		con = mysql_init(con);
		
		if (con == NULL)
		{
			LOG_ERROR("mysql_init() error");
			exit(1);
		}
		con = mysql_real_connect(con, m_url.c_str(), m_user.c_str(), m_password.c_str(), m_databaseName.c_str(), m_port, NULL, 0);
		
		if (con == NULL)
		{
			LOG_ERROR("mysql_real_connect() error");
			exit(1);
		}
		connList.push_back(con);
		++m_freeConn;
    }
    LOG_INFO("数据库池创建成功。。。。。。。。");
    reserve = sem(m_freeConn);
    
}

//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL* connection_pool::getConnection()
{
    MYSQL* con = NULL;

    if(0 == connList.size())
    {
        fputs("数据库连接池为空\n",stdout);
        return NULL;
    }

    reserve.wait();
    lock.lock();
    con = connList.front();
    connList.pop_front();

    --m_freeConn;
    ++m_curConn;

    lock.unlock();
    return con;
}

//释放当前使用的连接
bool connection_pool::releaseConnection(MYSQL* con)
{
    if(NULL == con)
    {
        return false;
    }
    lock.lock();
    connList.push_back(con);
    ++m_freeConn;
    --m_curConn;

    lock.unlock();
    reserve.post();
    return true;
}

//销毁数据库连接池
void connection_pool::destoryPool()
{
    lock.lock();
    if(connList.size() > 0)
    {
        list<MYSQL*>::iterator it;
        for(it = connList.begin();it != connList.end();++it)
        {
            MYSQL* con = *it;
            mysql_close(con);
        }
        m_curConn = 0;
        m_freeConn = 0;
        connList.clear();
    }
    lock.unlock();
}

//当前空闲连接数
int connection_pool::getFreeConn()
{
    return m_freeConn;
}

connectionRAII::connectionRAII(MYSQL ** SQL,connection_pool *connPool)
{
    *SQL = connPool -> getConnection();

    m_conn = *SQL;
    m_poolRAII = connPool;
}

connectionRAII::~connectionRAII()
{
    m_poolRAII -> releaseConnection(m_conn);
}
