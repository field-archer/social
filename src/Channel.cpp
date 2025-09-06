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
        int TYPE_=-1;
        std::string name_="";
        std::string message_="";
        HandleHttp(TYPE_,name_,message_);
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
void Channel::HandleHttp(int& _TYPE,std::string& _name,std::string& _message)
{
    //TODO，
    char buffer[4096];
    ssize_t bytes_read = read(fd_, buffer, sizeof(buffer)-1);
    if (bytes_read <= 0) {
        _TYPE = -1;
        return;
    }
    buffer[bytes_read] = '\0';

    // 解析HTTP请求行
    char method[32] = {0};
    char url[1024] = {0};
    sscanf(buffer, "%31s %1023s", method, url);

    // 查找消息体开始位置
    char* body_start = strstr(buffer, "\r\n\r\n");
    if (!body_start) {
        _TYPE = -1;
        return;
    }
    body_start += 4;

    // 解析查询参数或消息体
    char* params = nullptr;
    if (strcasecmp(method, "GET") == 0) {
        params = strchr(url, '?');
        if (params) *params++ = '\0';
    } else if (strcasecmp(method, "POST") == 0) {
        params = body_start;
    }

    // 提取参数
    if (params) {
        char* token = strtok(params, "&");
        while (token) {
            char* eq = strchr(token, '=');
            if (eq) {
                *eq = '\0';
                if (strcmp(token, "type") == 0) {
                    _TYPE = atoi(eq+1);
                } else if (strcmp(token, "username") == 0) {
                    _name = eq+1;
                } else if (strcmp(token, "content") == 0) {
                    _message = eq+1;
                }
            }
            token = strtok(nullptr, "&");
        }
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
void Channel::Send(std::string message)                                 //发送消息
{
    MessageToHttp(message);
    int sendn=::send(fd_,message.data(),message.size(),0);
}
void Channel::MessageToHttp(std::string& message)
{
    //TODO
    std::stringstream http_response;
    http_response << "HTTP/1.1 200 OK\r\n"
                  << "Content-Type: text/plain\r\n"
                  << "Content-Length: " << message.length() << "\r\n"
                  << "Connection: close\r\n"
                  << "\r\n"
                  << message;
    
    message = http_response.str();
}






void Channel::SetHandleWriteEvent(std::function<void()> _fun)
{
    HandleWriteEvent=_fun;
}
void Channel::SetRevent(uint32_t _event)                       //设置发生的事件
{
    Revent_=_event;
}