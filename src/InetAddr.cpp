#include"InetAddr.h"
InetAddr::InetAddr(std::string _ip,uint16_t _port)
{
    addr_.sin_family=AF_INET;
    addr_.sin_addr.s_addr=inet_addr(_ip.c_str());
    addr_.sin_port=htons(_port);
}
InetAddr::InetAddr()
{
    memset(&addr_,0,sizeof(addr_));
    addr_.sin_family=AF_INET;
}
InetAddr::~InetAddr()
{

}
sockaddr* InetAddr::addr()
{
    return (sockaddr*)&addr_;
}
std::string InetAddr::ip()
{
    char IP[INET_ADDRSTRLEN]="";
    inet_ntop(AF_INET,&addr_.sin_addr,IP,sizeof(IP));
    return std::string(IP);
}
uint16_t InetAddr::port()
{
    return ntohs(addr_.sin_port);
}