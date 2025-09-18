#pragma once
#include<string.h>
#include<functional>
#include<memory>
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
    EventLoop* eventLoop_;                                                      //生命周期和TcpServer一样，不析构
    Socket servSocket;                                                         //生命周期和Acceptor一样，析构
    Channel servChannel;                                                       //生命周期和Acceptor一样，析构
    std::function<void(std::unique_ptr<Socket> clieSocket)> newConnectionCB;                    //将客户端socket传递至TcpServer
public:
    Acceptor(std::string _ip,uint16_t _port,EventLoop* _eventLoop);             //构造函数
    ~Acceptor();                                                                //析构函数

    void SetNewConnectionCB(std::function<void(std::unique_ptr<Socket> clieSocket)> _fun);      //绑定acceptor处理新连接的回调函数
    void NewConnection();                                                       //处理新连接，产生客户端socket并通过回调函数传递至TcpServer
};