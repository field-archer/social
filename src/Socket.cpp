#include"Socket.h"
Socket::Socket(std::string _ip, uint16_t _port)                         //监听socket构造函数
{
    fd_=::socket(AF_INET,SOCK_STREAM,0);
    if(fd_==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror("创建服务端socket失败:");
        exit(-1);
    }
    ip_=_ip;
    port_=_port;               
}
Socket::Socket(int _fd,std::string _ip,uint16_t _port)                  //客户端socket构造函数
{
    fd_=_fd;
    ip_=_ip;
    port_=_port;
}
Socket::~Socket()                                                       //析构函数
{
    //fd交由channel管理
    //无需析构
}
void Socket::SetNonBlock()                                              //非阻塞
{
    int flags=fcntl(fd_,F_GETFL,0);
    flags|=O_NONBLOCK;
    if(fcntl(fd_,F_SETFL,flags)==-1)
    {
        printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd_);
        perror("设置非阻塞套接字失败:");
    }
}
void Socket::SetReuseAddr()                                             //重用地址，避免time_wait状态
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag));
}
void Socket::SetReusePort()                                             //重用端口，避免惊群效应，在reactor中意义不大
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&flag,sizeof(flag));
}
void Socket::SetTcpNoDelay()                                            //避免小数据包延迟
{
    int flag=1;
    setsockopt(fd_,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(flag));
}
void Socket::SetKeepAlive()                                             //避免僵尸进程
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&flag,sizeof(flag));
}

void Socket::bind(InetAddr _addr)                                       //绑定ip和端口  bind
{
    if(::bind(fd_,_addr.addr(),sizeof(_addr))==-1)
    {
        printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd_);
        perror("绑定服务端socket失败:");
        exit(-1);
    }
}   
void Socket::listen(int len)                                            //设置监听      listen
{
    if(::listen(fd_,len)==-1)
    {
        printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd_);
        perror("设置监服务端监听socket失败:");
        exit(-1);
    }
}
int Socket::accept(InetAddr& _clieAddr)                                  //受理连接      accept
{
    memset(&_clieAddr,0,sizeof(_clieAddr));
    socklen_t addrLen=sizeof(_clieAddr.addr());
    int clie_fd=::accept(fd_,_clieAddr.addr(),&addrLen);
    if(clie_fd==-1)
    {
        printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd_);
        perror("accept客户端失败:");
    }
    return clie_fd;
}
int Socket::fd()                                                        //fd接口
{
    return fd_;
}
std::string Socket::ip()                                                //ip接口
{
    return ip_;
}
uint16_t Socket::port()                                                 //port接口
{
    return port_;
}