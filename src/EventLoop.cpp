#include"EventLoop.h"
EventLoop::EventLoop():epoll_(new Epoll())
{   
}
EventLoop::~EventLoop()
{
}
void EventLoop::run()                                   
{
    while(true)
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

