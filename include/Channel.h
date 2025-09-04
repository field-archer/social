#pragma once
#include<functional>
#include<sys/socket.h>
#include<unistd.h>
#include"http.h"
#include"EventLoop.h"               //eventLoop和epoll均只有一个，用eventloop替代epoll，以便在tcpserver中使用eventLoop替代epoll
class Epoll;
class EventLoop;
class http;

class Channel
{
private:
    int fd_;                                //fd由Channel管理
    EventLoop *eventLoop_;                  //eventLoop不属于channel，无需析构
    uint32_t Tevent_;                       //关注的事件
    uint32_t Revent_;                       //实际发生的事件，需手动维护，便于事件处理
    bool inEpoll_=false;                    //是否在epoll_，需手动维护，用于区分update时的mod和add

public:
    std::function<void()>HandleReadEvent;   //处理读事件的回调函数
    std::function<void(int)>HandleCloseEvent;//处理断开连接事件
    Channel(int _fd,EventLoop *_eventLoop); //构造函数
    ~Channel();                             //析构函数：fd

    void HandleEvent();                     //处理事件
    void DataChange();                      //处理正常读事件，数据收发
    
    void SetRevent(uint32_t _event);        //设置发生的事件
    void SetInEpoll();                      //设置inEpoll
    void EnableReading();                   //使该channel加入事件循环，并设置可读
    void SetHandleReadEvent(std::function<void()>);     //设置处理读事件的回调函数
    void SetHandleCloseEvent(std::function<void(int)>); //设置处理断开连接的回调函数
    void UseEt();                           //使用边缘触发

    bool inEpoll();                         //返回inEpoll
    uint32_t Tevent();                      //返回Tevent
    uint32_t Revent();                      //返回Revent
    int fd();                               //返回fd
    
};