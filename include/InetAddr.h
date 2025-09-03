#pragma once
#include<string>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
class InetAddr
{
private:
    sockaddr_in addr_;                          //sockaddr_in
public:
    InetAddr(std::string _ip,uint16_t port);    //服务端构造函数
    InetAddr();                                 //客户端构造函数
    ~InetAddr();                                //析构函数，无需析构

    sockaddr* addr();                           //sockaddr*接口，返回sockaddr*
    std::string ip();                           //ip接口
    uint16_t port();                            //port接口
};