#include"Connectio.h"
Connection::Connection(int _fd,EventLoop *_eventLoop,Socket *_clieSocket)
{
    eventLoop_=_eventLoop;                                                              //已有eventLoop
    clieSocket_=_clieSocket;                                                            //管理本不该有的客户端Socekt
    clieChannel_=new Channel(_fd,_eventLoop);                                           //新Channel
    clieChannel_->UseEt();                                                              //边缘触发
    clieChannel_->EnableReading();                                                      //注册可读
    clieChannel_->SetHandleReadEvent(std::bind(&Channel::DataChange,clieChannel_));     //绑定读事件处理：Channel的数据处理
}
Connection::~Connection()
{
    delete clieSocket_;                                                                 //析构本不该有的客户端Socekt
    delete clieChannel_;                                                                //析构Channel
}
void Connection::SetChannelCloseCB(std::function<void(int)> _fun)                       //设置channel处理关闭事件的函数
{
    clieChannel_->SetHandleCloseEvent(_fun);
}