#pragma once
#include<vector>
#include<memory>
#include"Epoll.h"
#include"Channel.h"
class Channel;
class Epoll;

class EventLoop
{
private:
    std::unique_ptr<Epoll> epoll_;                                      //生命周期和EventLoop一样，析构
public:
    EventLoop();                                        //构造函数
    ~EventLoop();                                       //析构函数：epoll_

    void run();                                         //事件循环
    void UpdateChannel(Channel *ch);                    //调用Epoll的update
    void removeChannel(Channel *ch);                    //清楚Channel
};