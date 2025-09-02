#include"Socket.h"
Socket::Socket(std::string _ip, uint16_t _port)                       //监听socket构造函数
{
    fd_=::socket(AF_INET,SOCK_STREAM,0);
    if(fd_==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(":");
        exit(-1);
    }
    ip_=_ip;
    port_=_port;               
}
Socket::Socket(int _fd,InetAddr clieAddr)
{
    fd_=_fd;
    ip_=clieAddr.ip();
    port_=clieAddr.port();
}
Socket::Socket()
{
    
}
Socket::~Socket()                                               //析构函数
{

}
void Socket::SetNonBlock()                                             //非阻塞
{
    int flags=fcntl(fd_,F_GETFL,0);
    flags|=O_NONBLOCK;
    if(fcntl(fd_,F_SETFL,flags)==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(":");
        exit(-1);
    }
}
void Socket::SetReuseAddr()                                            //重用地址，避免time_wait状态
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag));
}
void Socket::SetReusePort()                                            //重用端口，避免惊群效应，在reactor中意义不大
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&flag,sizeof(flag));
}
void Socket::SetTcpNoDelay()                                           //避免小数据包延迟
{
    int flag=1;
    setsockopt(fd_,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(flag));
}
void Socket::SetKeepAlive()                                            //避免僵尸进程
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&flag,sizeof(flag));
}

void Socket::bind(InetAddr _addr)                                //绑定ip和端口
{
    if(::bind(fd_,_addr.addr(),sizeof(_addr))==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(":");
        exit(-1);
    }
}   
void Socket::listen(int len)                              //设置监听
{
    if(::listen(fd_,len)==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(":");
        exit(-1);
    }
}
int Socket::accept(InetAddr clieAddr_)                              //受理连接
{
    socklen_t addrLen=sizeof(clieAddr_.addr());
    int clie_fd=::accept(fd_,clieAddr_.addr(),&addrLen);
    if(clie_fd==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(":");
        // exit(-1);
    }
    return clie_fd;
}
int Socket::fd()
{
    return fd_;
}