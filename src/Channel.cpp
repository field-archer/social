#include"Channel.h"
Channel::Channel(int _fd,EventLoop *_eventLoop) //构造函数
{
    fd_=_fd;
    eventLoop_=_eventLoop;
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
    // if(Revent()&EPOLLIN)             //读事件
    // {
    //     HandleReadEvent();
    // }else if(Revent()&EPOLLRDHUP)   //客户端关闭                            
    // {
    //     printf("关闭事件\n");
    //     HandleCloseEvent(fd_);
    // }else 
    // {
    //     printf("发生其余事件：%d\n",Revent());
    // }


    if(Revent()&EPOLLRDHUP)   //客户端关闭                            
    {
        printf("关闭事件\n");
        HandleCloseEvent(fd_);
    }else if(Revent()&EPOLLIN)             //读事件
    {
        printf("正常读事件\n");
        HandleReadEvent();
    }else 
    {
        printf("发生其余事件：%d\n",Revent());
    }


}
void Channel::SetHandleReadEvent(std::function<void()> _fun)//设置处理读事件的回调函数
{
    HandleReadEvent=_fun;
}
void Channel::SetHandleCloseEvent(std::function<void(int)> _fun)
{
    HandleCloseEvent=_fun;
}
void Channel::DataChange()                      //处理正常读事件，数据收发
{
    //现在客户端退出后服务端也会推出，估计是只有send且还没通过socket
    char sendBuf[10]="aaa\n";
    printf("开始DataChange\n");
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