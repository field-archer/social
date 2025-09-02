#pragma once
#include"EventLoop.h"               //eventLoop和epoll均只有一个，用eventloop替代epoll，以便在tcpserver中使用eventLoop替代epoll
#include<functional>
#include<sys/socket.h>
class Epoll;
class EventLoop;

class Channel
{
private:
    int fd_;
    EventLoop *eventLoop_;                  //eventLoop不属于channel，无需析构
    uint32_t Tevent_;
    uint32_t Revent_;
    bool inEpoll_=false;

public:
    std::function<void()>HandleReadEvent;   //处理读事件的回调函数
    Channel(int _fd,EventLoop *_eventLoop); //构造函数
    ~Channel();                             //析构函数

    void HandleEvent();                     //处理事件
    void DataChange();                      //处理正常读事件，数据收发
    
    void SetRevent(uint32_t _event);                       //设置发生的事件
    void SetInEpoll();                      //设置inEpoll
    void EnableReading();                   //使该channel加入事件循环，并设置可读
    void SetHandleReadEvent(std::function<void()>);              //设置处理读事件的回调函数
    void UseEt();                           //使用边缘触发

    bool inEpoll();                         //返回inEpoll
    uint32_t Tevent();                      //返回Tevent
    uint32_t Revent();                      //返回Revent
    int fd();                               //返回fd
    
};