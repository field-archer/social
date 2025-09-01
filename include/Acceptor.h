#pragma once
#include"Socket.h"
class Acceptor
{
private:
    Socket servSocket;
public:
    Acceptor(string _ip,uint16_t _port);
    ~Acceptor();
};