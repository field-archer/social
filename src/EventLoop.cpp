#include"EventLoop.h"
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
EventLoop::EventLoop(bool _mainLoop,int _timeval,int _timeout):timeval_(_timeval),timeout_(_timeout),
            epoll_(new Epoll()),
            wakefd_(eventfd(0,EFD_NONBLOCK)),wakeChannel_(new Channel(wakefd_,this)),
            tfd_(createtimerfd(_timeval)),timeChannel_(new Channel(tfd_,this)),
            mainLoop_(_mainLoop),stop_(false)
{   
    wakeChannel_->SetHandleReadEvent(std::bind(&EventLoop::HandleWake,this));
    wakeChannel_->EnableReading();
    timeChannel_->SetHandleReadEvent(std::bind(&EventLoop::HandleAlarm,this));
    timeChannel_->UseEt();
    timeChannel_->EnableReading();
}
EventLoop::~EventLoop()
{
}
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
void EventLoop::UpdateChannel(Channel *ch)              //调用Epoll的update
{
    epoll_->UpdateChannel(ch);
}
//清楚Channel
void EventLoop::removeChannel(Channel *ch)                    
{
    epoll_->removeChannel(ch);
}

//判断当前线程是否为事件循环线程
bool EventLoop::isLoopThread()                                
{
    return threadID_==syscall(SYS_gettid);          //判断是否为IO线程
}

void EventLoop::wakeUp()
{
    uint64_t tmp=1;
    ::write(wakefd_,&tmp,sizeof(tmp));
}
void EventLoop::HandleWake()
{
    uint64_t tmp=0;
    ::read(wakefd_,&tmp,sizeof(tmp));

    std::function<void()> fun_;

    std::lock_guard<std::mutex> lock(mutex_);

    while(!taskQueue_.empty())
    {
        fun_=taskQueue_.front();
        taskQueue_.pop();
        fun_();
    }
}

void EventLoop::addTaskLoop(std::function<void()> _fun)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskQueue_.push(_fun);
    }

    wakeUp();//唤醒IO线程
}
void EventLoop::HandleAlarm()
{
    
    uint64_t tmp;
    int readn=::read(tfd_,&tmp,sizeof(tmp));
    if(readn!=8)
    {
        printf("读取失败\n");
        return ;
    }
    if(mainLoop_)
    {

    }else 
    {
        for(auto it=connections_.begin();it!=connections_.end();)
        {
            if(it->second->timeOut(time(0),timeout_))
            {
                printf("删除%d\n",it->first);
                timeOutCB(it->first);
                {
                    std::lock_guard<std::mutex> lock(mmutex_);
                    it=connections_.erase(it);
                }
            }else 
            {
                ++it;
            }
        }
    }
}
void EventLoop::newConnection(spConnection _newConnection)
{
    connections_[_newConnection->fd()]=_newConnection;
}
void EventLoop::SetTimeOutCB(std::function<void(int)> _fun)
{
    timeOutCB=_fun;
    mutex_;
}
void EventLoop::Stop()
{
    stop_=true;
    wakeUp();
}