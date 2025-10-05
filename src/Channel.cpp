#include"Channel.h"
Channel::Channel(int _fd,EventLoop *_eventLoop):fd_(_fd),eventLoop_(_eventLoop)
{
}
Channel::~Channel()                             //析构函数
{
    if(fd_==-1)return ; //避免close(-1)
    if(close(fd_)==-1)
    {
        printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd_);
        perror("close fd失败:");
        return ;
    }
    fd_=-1;             //避免重复close
}
void Channel::EnableReading()                   //使该channel加入事件循环，并设置可读
{
    Tevent_|=EPOLLIN;               //关注可读
    eventLoop_->UpdateChannel(this);//注册       
}
void Channel::DisableReading()
{
    Tevent_&=~EPOLLIN;
    eventLoop_->UpdateChannel(this);
}
void Channel::EnableWriting()
{
    Tevent_|=EPOLLOUT;
    eventLoop_->UpdateChannel(this);
}
void Channel::DisableWriting()
{
    Tevent_&=~EPOLLOUT;
    eventLoop_->UpdateChannel(this);
}
void Channel::DisableAll()                      //注销所有事件
{
    Tevent_&=0;
    eventLoop_->UpdateChannel(this);
}
void Channel::remove()                          //从epoll和eventLoop中清除Channel
{
    DisableAll();//取消关注事件所有事件
    eventLoop_->removeChannel(this);//清除Channel
}
void Channel::SetInEpoll()                      //设置inEpoll
{
    inEpoll_=true;
}
void Channel::UseEt()                           //使用边缘触发
{
    Tevent_|=EPOLLET;
}    

bool Channel::inEpoll()                         //返回inEpoll
{ 
    return inEpoll_;
}
uint32_t Channel::Tevent()                      //返回Tevent
{
    return Tevent_;
}
uint32_t Channel::Revent()                      //返回Revent
{
    return Revent_;
}
int Channel::fd()                               //返回fd
{
    return fd_;
}

void Channel::HandleEvent()                     //处理事件
{
    if(Revent()&EPOLLRDHUP)                     //关闭事件                           
    {
        remove();
        HandleCloseEventCB(fd_);
    }else if(Revent()&EPOLLIN)                  //读事件
    {
        HandleReadEventCB();
    }else if (Revent()&EPOLLOUT)                //可写
    {
        HandleWriteEventCB();
    }else                                        //其余事件一律关闭
    {
        printf("发生其余事件：%d\n",Revent());
        remove();
        HandleCloseEventCB(fd_);
    }
}
void Channel::SetHandleReadEvent(std::function<void()> _fun)            //设置读回调函数
{
    HandleReadEventCB=_fun;
}
void Channel::SetHandleCloseEvent(std::function<void(int)> _fun)        //设置关闭回调函数
{
    HandleCloseEventCB=_fun;
}
void Channel::SetHandleWriteEvent(std::function<void()> _fun)                        //设置写回调函数
{   
    HandleWriteEventCB=_fun;
}

void Channel::SetRevent(uint32_t _event)                       //设置发生的事件
{
    Revent_=_event;
}
