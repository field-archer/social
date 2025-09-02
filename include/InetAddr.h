#pragma once
#include<string>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
class InetAddr
{
private:
    sockaddr_in addr_;
public:
    InetAddr(std::string _ip,uint16_t port);
    InetAddr();
    ~InetAddr();
    sockaddr* addr();
    std::string ip();
    uint16_t port();
};