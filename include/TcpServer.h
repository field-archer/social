#pragma once
#include<functional>
#include"Acceptor.h"
#include"EventLoop.h"
#include"Connectio.h"
class TcpServer
{
private:
    Acceptor *acceptor_;                    //初始化工作
    EventLoop *eventLoop_;                  //事件循环
    functional<void()> newConnectionCB;     //在tcpserver中绑定Connection的初始化，acceptor处理新连接时调用该回调函数
public:
    TcpServer(string _ip,uint16_t _port);                            //构造函数
    ~TcpServer();                                                  //析构函数

    void Start();                                                  //处理事件

    void SetNewConnectionCB(functional<void()> _fun);                   //绑定acceptor处理新连接的回调函数
};