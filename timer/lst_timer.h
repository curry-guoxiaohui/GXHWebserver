#ifndef LST_TIMER
#define LST_TIMER


#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>


#include <time.h>
#include "../log/log.h"
/*定时器节点类 前置声明*/
class util_timer;
/*客户端信息*/
struct client_data
{
    sockaddr_in address;
    int sockfd;
    util_timer *timer;
};
/*定时器节点类*/
class util_timer
{
public:
    util_timer():prev(NULL),next(NULL){}

public:
    time_t expire;

    void (* cb_func)(client_data*);
    client_data* user_data;
    util_timer* prev;
    util_timer* next;
};
/*双向链表类 每一个节点是定时器节点*/
class sort_timer_lst
{
public:
    sort_timer_lst();
    ~sort_timer_lst();

    void add_timer(util_timer* timer);
    void adjust_timer(util_timer* timer);
    void del_timer(util_timer* timer);
    void tick();
private:
    void add_timer(util_timer* timer,util_timer* lst_head);

    util_timer* head;
    util_timer* tail;
};



#endif