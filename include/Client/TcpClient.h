#pragma once
#include<string>
#include<memory>
#include<stdexcept>
#include<sys/socket.h>
#include<fcntl.h>
#include"InetAddr.h"

//与服务端建立Tcp连接

class TcpClient
{
private:
    int sockfd_;            //socket

    std::string ip_;        //服务端ip
    uint16_t port_;      //服务端port
    InetAddr addr_;         //服务端地址类
    

    void Connect();         //连接服务端
    void SetNonBlock();     //非阻塞
    // void Start();           //开始于服务端通讯，一发一收
public:
    TcpClient(const std::string& _ip,uint16_t _port);
    void Recv(std::string& _message);               //接收数据
    void Send(const std::string& _message);         //发送数据
};