#pragma once
#include"TcpServer.h"
#include"ThreadPool.h"
#include<string>
class TcpServer;
class ThreadPool;



class EchoTcpServer
{
private:
    TcpServer tcpServer_;
    ThreadPool workThreadPool_;
public:
    EchoTcpServer(std::string _ip,uint16_t _port,int _eventLoopNum=5,int workNum=5);                     //构造函数
    ~EchoTcpServer();                                                                   //析构函数
    void Start();                                                                       //处理事件
 
    void NewConnection(Socket *clieSocket);                                             //处理新连接，将fd交给Connection，处理客户端ip和port
    void Close(int _fd);                                                                //根据fd关闭Connection，map中也删除
    void HandleMessage(spConnection _connection,std::string _message);                    //处理消息
    void OnMessage(spConnection _connection,std::string _message);                                               //计算
};