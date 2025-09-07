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
        printf("关闭事件\n");
        HandleCloseEvent(fd_);
    }else if(Revent()&EPOLLIN)                  //读事件
    {
        if(isListen)
        {
            HandleReadEvent();
            return ;
        }


        Http http;
        http.reset();
        char buffer[1024];
        memset(buffer,0,sizeof(buffer));
        int recvn = ::recv(fd_, buffer, sizeof(buffer), 0);
        printf("收到:%s\n",buffer);
        int rc = http.parse_raw(buffer, recvn);
        if(rc!=0)
        {
            printf("解析http出错\n");
            return ;
        }
        int TYPE_=http.type;
        std::string name_=http.get_username();
        std::string message_=http.get_text();
        std::cout<<"username="<<name_<<",type="<<TYPE_<<",message="<<message_<<endl;

        if(TYPE_==LOGIN)
        {
            HandleLogInEvent(name_);            //用户登录
        }else if(TYPE_==LOGOUT)
        {
            HandleLogOutEvent();                //用户登出
        }else if(TYPE_==MESSAGE)
        {
            HandleMessageEvent(message_);       //消息事件
        }
    }else                                       //其余事件一律关闭
    {
        printf("发生其余事件：%d\n",Revent());
        HandleCloseEvent(fd_);
    }
}
void Channel::SetHandleReadEvent(std::function<void()> _fun)            //设置处理读事件的回调函数
{
    HandleReadEvent=_fun;
}
void Channel::SetHandleCloseEvent(std::function<void(int)> _fun)        //设置处理关闭事件的回调函数
{
    HandleCloseEvent=_fun;
}
void Channel::SetHandleLogInEvent(std::function<void(std::string _name)> _fun)      //设置处理登录事件的回调函数
{
    HandleLogInEvent=_fun;
}
void Channel::SetHandleLogOutEvent(std::function<void()> _fun)                       //设置处理用户登出的回调函数
{
    HandleLogOutEvent=_fun;
}
void Channel::SetHandleMessageEvent(std::function<void(std::string)> _fun)           //设置处理消息事件的回调函数
{
    HandleMessageEvent=_fun;
}
// // 发送示例
// std::string resp = Http::build_response("alice", MESSAGE, "hello world");
// send(client_sock, resp.data(), (int)resp.size(), 0);
void Channel::Send(std::string name,std::string message)                                 //发送消息
{
    string tmpmessage=Http::build_response(name.c_str(),MESSAGE,message.c_str());
    int sendn=::send(fd_,tmpmessage.data(),tmpmessage.size(),0);
}




void Channel::SetHandleWriteEvent(std::function<void()> _fun)
{
    HandleWriteEvent=_fun;
}
void Channel::SetRevent(uint32_t _event)                       //设置发生的事件
{
    Revent_=_event;
}