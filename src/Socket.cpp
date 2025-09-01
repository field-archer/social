#include"Socket.h"
Socket::Socket(string _ip,uint16_t _port)                       //监听socket构造函数
{
    addr_=InetAddr(_ip,_port);                  
}
Socket::Socket()                                                //客户端socket构造函数
{
    memset(&addr_,0,sizeof(addr_));
}
Socket::~Socket()                                               //析构函数
{

}