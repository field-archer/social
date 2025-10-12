#include"EventLoop.h"
//返回一个周期_sec秒的定时器fd(默认3s)
int createtimerfd(int _sec=3)
{
    int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    itimerspec timeout;
    memset(&timeout,0,sizeof(timeout));
    timeout.it_value.tv_sec=_sec;
    timeout.it_interval.tv_sec=_sec;
    timerfd_settime(tfd,0,&timeout,NULL);
    return tfd;
}
EventLoop::EventLoop(bool _mainLoop,int _timeval,int _timeout):
            timeval_(_timeval),timeout_(_timeout),//每隔timeval_处理超过timeout_无信息交换的连接
            epoll_(new Epoll()),
            wakefd_(eventfd(0,EFD_NONBLOCK)),wakeChannel_(new Channel(wakefd_,this)),
            tfd_(createtimerfd(_timeval)),timeChannel_(new Channel(tfd_,this)),
            mainLoop_(_mainLoop),stop_(false)
{   
    //wakeChannel实际处理可写事件
    wakeChannel_->SetHandleReadEvent(std::bind(&EventLoop::HandleWake,this));
    wakeChannel_->EnableReading();
    
    //timeChannel处理超时事件
    timeChannel_->EnableReading();
    timeChannel_->SetHandleReadEvent(std::bind(&EventLoop::HandleAlarm,this));
    timeChannel_->UseEt();
}
EventLoop::~EventLoop()
{
}
//run运行在IO线程中
void EventLoop::run()                                   
{
    threadID_=syscall(SYS_gettid);      //存储IO线程ID
    while(!stop_)
    {
        std::vector<Channel*>channel=epoll_->loop(-1);  
        for(auto& ch:channel)
        {
            ch->HandleEvent();                          //事件处理
        }
    }
}
//调用Epoll的update
void EventLoop::UpdateChannel(Channel *ch)             
{
    epoll_->UpdateChannel(ch);
}
//清除Channel
void EventLoop::removeChannel(Channel *ch)                    
{
    epoll_->removeChannel(ch);
}

//判断当前线程是否为IO线程
bool EventLoop::isLoopThread()                                
{
    return threadID_==syscall(SYS_gettid);          //判断是否为IO线程
}
//写入管道，Epoll自动唤醒
void EventLoop::wakeUp()
{
    uint64_t tmp=1;
    ::write(wakefd_,&tmp,sizeof(tmp));
}
//处理唤醒
void EventLoop::HandleWake()
{
    //读取避免ET出错
    uint64_t tmp=0;
    ::read(wakefd_,&tmp,sizeof(tmp));

    //接收任务函数
    std::function<void()> fun_;

    std::lock_guard<std::mutex> lock(mutex_);//加锁，因为函数运行在IO线程，主线程addTaskLoop也会操作taskQueue

    while(!taskQueue_.empty())
    {
        fun_=taskQueue_.front();
        taskQueue_.pop();
        //执行任务
        fun_();
    }
}

void EventLoop::addTaskLoop(std::function<void()> _fun)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskQueue_.push(_fun);
    }

    wakeUp();//写入事件，唤醒IO线程
}
//响应定时器
void EventLoop::HandleAlarm()
{
    //读取定时器
    uint64_t tmp;
    int readn=::read(tfd_,&tmp,sizeof(tmp));
    if(readn!=8)
    {
        printf("读取失败\n");
        return ;
    }
    //主事件循环connection_是空，无需额外判断
    for(auto it=connections_.begin();it!=connections_.end();)
    {
        if(it->second->timeOut(time(0),timeout_))
        {
            printf("超时删除%d\n",it->first);
            printf("%d:%d\n",timeval_,timeout_);
            timeOutCB(it->first);//删除TcpServer connectionMap中对应元素
            {//需要加锁，因为主线程会调用EventLoop的newConnection操作connection_
                std::lock_guard<std::mutex> lock(mmutex_);
                it=connections_.erase(it);//erase返回下一个迭代器
            }
        }else 
        {
            ++it;//正常迭代
        }
    }
}
void EventLoop::newConnection(spConnection _newConnection)
{
    //加锁，因为该函数被TcpServer的NewConnection调用（主循环），子循环的超时机制handleAlarm也会操作connection_
    std::lock_guard<std::mutex> lock(mmutex_);
    connections_[_newConnection->fd()]=_newConnection;
}
//将超时通知至TcpServer
//删除TcpServer中的connection map
void EventLoop::SetTimeOutCB(std::function<void(int)> _fun)
{
    timeOutCB=_fun;
}
void EventLoop::Stop()
{
    stop_=true;
    wakeUp();
}
//删除map中fd对应Connection
void EventLoop::DelConnection(int _fd)
{
    std::lock_guard<std::mutex>lock(mutex_);//加锁，因为是IO线程处理事件时调用所以属于IO线程，主线程的NewConnection会操作connection_
    connections_.erase(_fd);
}