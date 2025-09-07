#include"Connectio.h"
Connection::Connection(int _fd,EventLoop *_eventLoop,Socket *_clieSocket)
{
    eventLoop_=_eventLoop;                                                              //已有eventLoop
    clieSocket_=_clieSocket;                                                            //管理本不该有的客户端Socekt
    clieChannel_=new Channel(_fd,_eventLoop);                                           //新Channel
    clieChannel_->UseEt();                                                              //边缘触发
    clieChannel_->EnableReading();                                                      //注册可读

    clieChannel_->SetHandleLogInEvent(std::bind(&Connection::HandleLogInEvent,this,std::placeholders::_1));     //绑定用户登录处理
    clieChannel_->SetHandleLogOutEvent(std::bind(&Connection::HandleLogOutEvent,this));                         //绑定登出处理
    clieChannel_->SetHandleMessageEvent(std::bind(&Connection::HandleMessageEvent,this,std::placeholders::_1)); //绑定消息处理
    clieChannel_->SetHandleCloseEvent(std::bind(&Connection::HandleCloseEvent,this));                           //绑定关闭连接处理
}

Connection::~Connection()
{
    delete clieSocket_;                                                                 //析构本不该有的客户端Socekt
    delete clieChannel_;                                                                //析构Channel
}
void Connection::HandleLogInEvent(std::string _name)
{
    name_=_name;
    if(name_=="")name_="名字不可以为空嗷！";
    std::cout<<"登陆了,name="<<name_<<endl;
}
void Connection::HandleLogOutEvent()
{
    HandleLogOutEventCB();
}
void Connection::SetHandleLogOutEvent(std::function<void()> _fun)
{
    HandleLogOutEventCB=_fun;
}
void Connection::HandleMessageEvent(std::string message)
{
    HandleMessageEventCB(message);
}
void Connection::SetHandleMessageEvent(std::function<void(std::string message)> _fun)
{
    HandleMessageEventCB=_fun;
}
void Connection::HandleCloseEvent()
{
    HandleCloseCB();
}
void Connection::SetHandleCloseEvent(std::function<void()> _fun)
{
    HandleCloseCB=_fun;
}

int Connection::fd()
{
    return clieChannel_->fd();
}
void Connection::send(std::string message)          //发送数据
{
    clieChannel_->Send(name_,message);
}
