#ifndef CONFIG_H
#define CONFIG
#include <unistd.h>
#include <string>
#include "../webserver/webserver.h"

using namespace std;

class Config
{
public:
    Config();
    ~Config(){};

    void parse_arg(int argc,char*argv[]);

    //端口号
    int PORT;

    //日志写入方式
    int LOGWrite;

    //触发组合方式
    int TRIGMode;

    //listenfd触发方式
    int LISTENTrigmode;

    //connfd触发方式
    int CONNTrigmode;

    //优雅关闭链接
    int OPT_LINGER;

    //数据库连接池最大数量
    int sql_num;

    //线程池内的线程数量
    int thread_num;

    //是否关闭日志
    int is_close_log;

    //并发模型选择
    int actor_model;
};

#endif