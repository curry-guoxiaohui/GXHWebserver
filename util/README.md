管理文件描述符 定时器事件的工具类
===
> 对文件描述符设置非阻塞（与epoll模式配合使用 需要将监听的文件描述符设置为非阻塞）
> 将内核事件表注册读事件，ET | LT模式，选择开启EPOLLONESHOT
> 从内核事件表删除文件描述符，并关闭文件描述符
> 从内核时间表修改文件描述符事件，主要是重新打开EPOLLONESHOT
> 信号处理函数
> 设置信号函数
> 定时处理任务，重新定时以不断触发SIGALRM信号 并定义定时任务的回调函数