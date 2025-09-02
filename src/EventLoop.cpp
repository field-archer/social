#include"EventLoop.h"
EventLoop::EventLoop()
{   
    epoll_=new Epoll();
}
EventLoop::~EventLoop()
{
    delete epoll_;
}
void EventLoop::run()
{
    while(true)
    {
        std::vector<Channel*>channel=epoll_->loop(-1);
        for(auto& ch:channel)
        {
            printf("正在处理一个事件\n");
            ch->HandleEvent();
        }
        printf("事件处理完毕\n");
    }
}
void EventLoop::UpdateChannel(Channel *ch)                    //调用Epoll的update
{
    epoll_->UpdateChannel(ch);
}
