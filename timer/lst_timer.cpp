#include "lst_timer.h"

sort_timer_lst::sort_timer_lst()
{
    head = NULL;
    tail = NULL;
}
sort_timer_lst::~sort_timer_lst()
{
    util_timer* tmp = head;
    while(tmp)
    {
        head = tmp -> next;
        delete tmp;
        tmp = head;
    }
}

void sort_timer_lst::add_timer(util_timer* timer)
{
    if(!timer)
    {
        return ;
    }
    if(!head)
    {
        head = tail = timer;
        return ;
    }
     /*将新加入的节点添加到链表头部*/
    if(timer -> expire < head -> expire)
    {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
    }
    add_timer(timer,head);
    LOG_INFO("%s","add_timer sussessful.....");
}
void sort_timer_lst::add_timer(util_timer* timer,util_timer* lst_head)
{
    util_timer* prev = lst_head;
    util_timer* tmp = prev->next;

    while(tmp)
    {
         /*将新加入的节点添加到链表中间*/
        if(timer->expire < tmp->expire)
        {
            prev->next = timer;
            timer->next = tmp;
            tmp->prev = timer;
            timer->prev = prev;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    /*将新加入的节点添加到链表尾部*/
    if(!tmp)
    {
        prev->next = timer;
        timer->prev = prev;
        timer->next = NULL;
        tail = timer;
    }
}

void sort_timer_lst::adjust_timer(util_timer* timer)
{
    if(!timer)
    {
        return;
    }
    util_timer* tmp = timer->next;
    //在尾部或者调整完时间后依然过期时间小于后一个节点的过期时间
    if(!tmp || (timer -> expire < tmp->expire))
    {
        return;
    }
    //在头部  先将该节点拿下来 再重新添加一遍
    if(timer == head)
    {
        head = head->next;
        head->prev = NULL;
        timer->next = NULL;
        add_timer(timer,head);
    }
    else{
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        timer->next = NULL;
        timer->prev = NULL;
        add_timer(timer,head);
    }
    LOG_INFO("%s","adjust_timer sussessful.....");
}

void sort_timer_lst::del_timer(util_timer* timer)
{
    if(!timer)
    {
        return;
    }
    if((timer == head) && (timer == tail))
    {
        delete timer;
        head = NULL;
        tail = NULL;
        return;
    }

    if(timer == head)
    {
        head = head->next;
        head->prev = NULL;
        delete timer;
        return;
    }
    if(timer == tail)
    {
        tail = tail->prev;
        tail->next = NULL;
        delete timer;
        return ;
    }

    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
    LOG_INFO("%s","del_timer sussessful.....");
}

void sort_timer_lst::tick()
{
    if(!head)
    {
        return;
    }

    time_t cur = time(NULL);
    util_timer* tmp = head;
    while(tmp)
    {
        if(cur < tmp->expire)
        {
            break;
        }

        tmp->cb_func(tmp->user_data);
        head = tmp->next;
        if(head)
        {
            head->prev = NULL;
        }
        delete tmp;
        tmp = head;
    }
}