#include"Channel.h"
Channel::Channel(int _fd,EventLoop *_eventLoop)                       //构造函数
{
    fd_=_fd;
    eventLoop_=_eventLoop;
}
Channel::~Channel()                             //析构函数
{
    
}
void Channel::EnableReading()                   //使该channel加入事件循环，并设置可读
{
    Tevent_|=EPOLLIN;
    eventLoop_->UpdateChannel(this);
}
void Channel::SetInEpoll()                      //设置inEpoll
{
    inEpoll_=true;
}
    
bool Channel::inEpoll()                         //返回inEpoll
{
    return inEpoll_;
}
uint32_t Channel::Tevent()                       //返回Tevent
{
    return Tevent_;
}
uint32_t Channel::Revent()                      //返回Revent
{
    return Revent_;
}
int Channel::fd()                                        //返回fd
{
    return fd_;
}
void Channel::UseEt()
{
    Tevent_|=EPOLLET;
}
void Channel::HandleEvent()                     //处理事件
{
    if(Revent()&EPOLLIN)                        //读事件
    {
        printf("开始处理读事件\n");
        HandleReadEvent();
    }else                                       //其余事件暂时全部按错误处理 
    {
        printf("发生其余事件\n");
    }
}
void Channel::SetHandleReadEvent(std::function<void()> _fun)              //设置处理读事件的回调函数
{
    HandleReadEvent=_fun;
}
void Channel::DataChange()                      //处理正常读事件，数据收发
{
    printf("正常读事件\n");
    char sendBuf[10]="aaa\n";
    int sendn=send(fd_,sendBuf,3,0);
    if(sendn<=0)
    {
        perror("send失败");
    }else 
    {
        printf("已成功回复aaa\n");
    }
    /*
    const int buffsize=1024;
    char recvBuf[buffsize]="";
    char sendBuf[buffsize]="";
    
    while(true)
    {
        int recvn=::recv(fd_,recvBuf,buffsize-1,0);
        if(recvn==-1)
        {
            printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
            perror(":");
            break;
        }else if(recvn==0)
        {
            printf("对方已关闭\n");
            break;
        }


    }
    */
}
void Channel::SetRevent(uint32_t _event)                       //设置发生的事件
{
    Revent_=_event;
}