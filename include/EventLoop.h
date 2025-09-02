#pragma once
#include<vector>
#include"Epoll.h"
#include"Channel.h"
class Channel;
class Epoll;

class EventLoop
{
private:
    Epoll *epoll_;                                      //生命周期和EventLoop一样，析构
public:
    EventLoop();                                        //构造函数
    ~EventLoop();                                       //析构函数

    void run();                                         
    void UpdateChannel(Channel *ch);                    //调用Epoll的update
};