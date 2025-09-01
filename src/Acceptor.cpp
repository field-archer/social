#include"Acceptor.h"
Acceptor::Acceptor(string _ip,uint16_t _port)
{
    servSocket=Socket(_ip,_port);               //初始化服务端socket
}