#pragma once
#include<netinet/in.h>
class InetAddr
{
private:
    sockaddr_in addr_;
public:
    InetAddr(string _ip,uint16_t port);
    InetAddr();
    ~InetAddr();
};