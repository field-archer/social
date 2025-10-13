#include"InetAddr.h"
InetAddr::InetAddr(std::string _ip,uint16_t _port)      //服务端socket（ip和port）
{
    addr_.sin_family=AF_INET;
    addr_.sin_addr.s_addr=inet_addr(_ip.c_str());
    addr_.sin_port=htons(_port);
}
InetAddr::InetAddr()                                    //客户端socket（空）
{
    memset(&addr_,0,sizeof(addr_));
    addr_.sin_family=AF_INET;
}
InetAddr::~InetAddr()   
{
    //无需析构
}
sockaddr* InetAddr::addr()                              //sockaddr*接口
{
    return (sockaddr*)&addr_;
}
std::string InetAddr::ip()                              //ip接口
{
    char IP[INET_ADDRSTRLEN]="";   
    inet_ntop(AF_INET,&addr_.sin_addr,IP,sizeof(IP));
    return std::string(IP);
}
uint16_t InetAddr::port()                               //port接口
{
    return ntohs(addr_.sin_port);
}