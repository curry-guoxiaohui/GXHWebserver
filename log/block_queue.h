/*************************************************************
*循环数组实现的阻塞队列，m_back = (m_back + 1) % m_max_size;  
*线程安全，每个操作前都要先加互斥锁，操作完后，再解锁
**************************************************************/
#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "../lock/locker.h"
using namespace std;

template<class T>
class block_queue
{
public:
    block_queue(int max_size = 1000)
    {
        if(max_size <= 0)
        {
            exit(-1);
        }
        m_max_size = max_size;
        m_array = new T[max_size];
        m_size = 0;
        m_front = -1;
        m_back = -1;
    }

    ~block_queue()
    {
        m_mutex.lock();
        if(m_array != NULL)
            delete[] m_array;      
        m_mutex.unlock();
    }

    void clear()
    {
        m_mutex.lock();
        m_size = 0;
        m_front = -1;
        m_back = -1;
        m_mutex.unlock();
    }

    //判断队列是否满了
    bool full()
    {
        m_mutex.lock();
        if(m_size >= m_max_size)
        {
            m_mutex.unlock();
            return true;
        }

        m_mutex.unlock();
        return false;
    }

    //判断队列是否为空
    bool empty()
    {
        m_mutex.lock();
        // 细节处理 将变量放于等号后面 防止少写一个等号变为赋值操作而不是相等操作
        if(0 == m_size)
        {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    //返回队首元素
    bool front(T& value)
    {
        m_mutex.lock();
        if(0 == m_size)
        {
            m_mutex.unlock();
            return false;
        }
        value = m_array[(m_front + 1) % m_max_size];
        m_mutex.unlock();
        return true;
    }
    //返回队尾元素
    bool back(T& value)
    {
         m_mutex.lock();
        if(0 == m_size)
        {
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_back];
        m_mutex.unlock();
        return true;
    }
    int size()
    {
        return m_size;
    }

    int max_size()
    {
        return m_max_size;
    }

    /**
     * 往队列里添加元素，需要将所有使用队列的线程先唤醒
     * 当有元素push进队列，相当于生产者生产了一个元素
     * 若当前没有线程等待条件变量 则唤醒无意义
    */
   bool push(const T& item)
   {
        // fputs("添加日志\n",stdout);
        // printf("--%d\n",m_size);
        m_mutex.lock();
        if (m_size >= m_max_size)
        {

            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }
        if(m_size >= m_max_size)
        {
            m_mutex.unlock();
            return false;
        }
        m_back = (m_back + 1) % m_max_size;
        m_array[m_back] = item;

        m_size++;
        m_cond.broadcast();
        m_mutex.unlock();
        return true;
   }

   //pop时,如果当前队列没有元素,将会等待条件变量
    bool pop(T &item)
    {

        m_mutex.lock();
        while (m_size <= 0)
        {
            if (!m_cond.wait(m_mutex.get()))
            {
                m_mutex.unlock();
                return false;
            }
        }

        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }

    //增加了超时处理
    bool pop(T& item, int ms_timeout)
    {
        struct timespec t = {0,0};
        struct timeval now = {0,0};
        gettimeofday(&now,NULL);
        m_mutex.lock();
        if(m_size <= 0)
        {
            t.tv_sec = now.tv_sec + ms_timeout / 1000;
            t.tv_nsec = (ms_timeout % 1000) * 1000;
            if (!m_cond.timewait(m_mutex.get(), t))
            {
                m_mutex.unlock();
                return false;
            }
        }
        if (m_size <= 0)
        {
            m_mutex.unlock();
            return false;
        }

        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    } 

private:
    /*实现同步*/
    locker m_mutex;
    cond m_cond;

    /*与维护阻塞队列有关的*/
    T*m_array;
    int m_size;
    int m_max_size;
    //队列 添加元素 back后移 指向最后一天添加的元素，删除元素，front后移 front指向第一个元素的前一个位置
    int m_front;
    int m_back;
};


#endif