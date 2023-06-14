#include "utils.h"


int *Utils::u_pipefd = 0;
int Utils::u_epollfd = 0;
void Utils::init(int timeslot)
{
    m_TIMESLOT = timeslot;
}

int Utils::setnonblocking(int fd)
{
    int old_optin = fcntl(fd,F_GETFL);
    int new_option = old_optin | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_optin;
}

void Utils::addfd(int epollfd,int fd,bool one_shot,int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;
    if(1 == TRIGMode)
    {
        event.events = EPOLLIN | EPOLLET | EPOLLHUP;
    }
    else{
        event.events = EPOLLIN | EPOLLHUP;
    }
    if(one_shot)
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnonblocking(fd);
}

//从内核时间表删除描述符
void Utils::removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

//将事件重置为EPOLLONESHOT
void Utils::modfd(int epollfd, int fd, int ev, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    else
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void Utils::sig_handle(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(u_pipefd[1],(char*)&msg,1,0);
    errno = save_errno;
}

void Utils::addsig(int sig,void(handler)(int),bool restart)
{
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = handler;
    if(restart)
    {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL) != -1);
}

/*定时处理任务，重新定时已不断触发SIGALRM信号*/
void Utils::timer_handler()
{
    m_timer_lst.tick();
    alarm(m_TIMESLOT);
}
void Utils::show_error(int connfd,const char* info)
{
    send(connfd,info,strlen(info),0);
    close(connfd);
}

class Utils;
void cb_func(client_data* user_data)
{
    assert(user_data);
    epoll_ctl(Utils::u_epollfd,EPOLL_CTL_DEL,user_data->sockfd,0);
    close(user_data->sockfd);
    http_conn::m_user_count--;
}
