#pragma once
#include"TcpServer.h"
#include<string>
class TcpServer;


class EchoTcpServer
{
private:
    TcpServer tcpServer_;
public:
    EchoTcpServer(std::string _ip,uint16_t _port);                                       //构造函数
    ~EchoTcpServer();                                                                   //析构函数
    void Start();                                                                       //处理事件
 
    void NewConnection(Socket *clieSocket);                                             //处理新连接，将fd交给Connection，处理客户端ip和port
    void Close(int _fd);                                                                //根据fd关闭Connection，map中也删除
    void HandleMessage(Connection *connection,std::string _message);                    //处理消息
};