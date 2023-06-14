#ifndef UTILS_
#define UTILS_
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <time.h>
#include "../log/log.h"
#include "../timer/lst_timer.h"
#include "../http/http_coon.h"

class Utils
{
public:
    Utils(){}
    ~Utils(){}

    void init(int timeslot);

    /*对文件描述符设置非阻塞*/
    static int setnonblocking(int fd);

    /*将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT*/
    static void addfd(int epollfd,int fd,bool one_shot,int TRIGMode);

    //从内核时间表删除描述符
    static void removefd(int epollfd, int fd);

    //将事件重置为EPOLLONESHOT
    static void modfd(int epollfd, int fd, int ev, int TRIGMode);

    /*设置处理函数*/
    static void sig_handle(int sig);

    /*设置信号函数*/
    static void addsig(int sig,void(handler)(int),bool restart = true);

    /*定时处理任务，重新定时已不断触发SIGALRM信号*/
    void timer_handler();

    static void show_error(int connfd,const char* info);

public:
    static int* u_pipefd;
    sort_timer_lst m_timer_lst;
    static int u_epollfd;
    int m_TIMESLOT;//定时器的时间间隔
};



void cb_func(client_data* user_data);


#endif