#pragma once
#include<string.h>
#include"Acceptor.h"
#include"EventLoop.h"
#include"Connectio.h"

class EventLoop;
class Acceptor;

class TcpServer
{
private:
    Acceptor *acceptor_;                    //初始化工作
    EventLoop *eventLoop_;                  //事件循环
public:
    TcpServer(std::string _ip,uint16_t _port);                            //构造函数
    ~TcpServer();                                                    //析构函数

    void Start();                                                    //处理事件
 
    void NewConnection(Socket *clieSocket);                                            //新连接
};