#pragma once
#include<string.h>
#include<functional>
#include"Socket.h"
#include"Channel.h"
#include"Epoll.h"
#include"EventLoop.h"

class Socket;
class Channel;
class EventLoop;

class Acceptor
{
private:
    Socket servSocket;
    Channel *servChannel;                                               //生命周期和EventLoop一样，不析构
    EventLoop *eventLoop_;                                                      //生命周期和EventLoop一样，不析构
    std::function<void(Socket *clieSocket)> newConnectionCB;                              //在tcpserver中绑定Connection的初始化，acceptor处理新连接时调用该回调函数
public:
    Acceptor(std::string _ip,uint16_t _port,EventLoop *_eventLoop);
    ~Acceptor();

    void SetNewConnectionCB(std::function<void(Socket *clieSocket)> _fun);                //绑定acceptor处理新连接的回调函数
    void NewConnection();                                               //处理新连接
};