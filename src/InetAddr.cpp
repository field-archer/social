#include"InetAddr.h"
InetAddr::InetAddr(string _ip uint16_t _port)
{
    addr_.sin_family=AF_INET;
    addr_.sin_addr.s_addr=_ip.cstr();
    addr_.sin_port=_port;
}
InetAddr::InetAddr()
{
    memset(&addr_,0,sizeof(addr_));
    addr_.sin_family=AF_INET;
}
InetAddr::~InetAddr()
{

}