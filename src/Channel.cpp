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
    if(Revent()&EPOLLRDHUP)                     //客户端关闭                            
    {
        printf("关闭事件\n");
        HandleCloseEvent(fd_);
    }else if(Revent()&EPOLLIN)                  //读事件
    {
        printf("正常读事件\n");
        HandleReadEvent();
    }else                                       //其余事件
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
    
    const int buffsize=1024;
    char recvBuf[buffsize]="";  //接收缓冲区
    char sendBuf[buffsize]="";  //发送缓冲区
    
    while(true)
    {
        int recvn=::recv(fd_,recvBuf,buffsize-1,0);//bufsize-1保证有'\0'位置
        if(recvn==-1)                              //出错
        {
            if(errno==EAGAIN||errno==EWOULDBLOCK)
            {
                printf("回显完毕\n");
                break;
            }else if(errno==EINTR)
            {
                printf("信号终端，重新接收\n");
                continue;
            }else if(errno==ENOTCONN)
            {
                printf("fd（%d）未连接\n",fd_);
                break;
            }else if(errno==EBADF)
            {
                printf("无效fd(%d)\n",fd_);
                break;
            }else 
            {
                printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd_);
                perror("接收客户端的数据出错:");
                break;
            }
        }else if(recvn==0)                          //对方关闭
        {
            printf("对方已关闭\n");
            HandleCloseEvent(fd_);
            break;
        }else                                       //正常 
        {   
            recvBuf[recvn]='\0';
            strncpy(sendBuf,recvBuf,buffsize);
            int sendn=::send(fd_,sendBuf,strlen(sendBuf),0);
            if(sendn==-1)                           //发送出错
            {
                if(errno==EPIPE)
                {
                    printf("连接（%d）已关闭\n",fd_);
                    break;
                }else if(errno==EMSGSIZE)
                {
                    printf("消息过大，不支持发送\n");
                    break;
                }else if(errno==EINTR)
                {
                    printf("信号终端，重试\n");
                    continue;
                }else 
                {
                    printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd_);
                    perror("发送数据出错:");
                    break;
                }
            }else if(sendn==0)                      //对方关闭
            {
                printf("对方已关闭\n");
                HandleCloseEvent(fd_);
                break;
            }else                                   //发送正常
            {
                printf("已回显发送%d字节\n",sendn);
            }
        }
    }
    
}
void Channel::SetRevent(uint32_t _event)                       //设置发生的事件
{
    Revent_=_event;
}