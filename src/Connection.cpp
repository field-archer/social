#include"Connectio.h"
Connection::Connection(int _fd,EventLoop *_eventLoop)
{
    eventLoop_=_eventLoop;
    clieChannel_=new Channel(_fd,_eventLoop);
    clieChannel_->UseEt();
    clieChannel_->EnableReading();
    clieChannel_->SetHandleReadEvent(std::bind(&Channel::DataChange,clieChannel_));
}
Connection::~Connection()
{
    delete clieChannel_;
}