#include"Socket.h"
Socket::Socket(string _ip,uint16_t _port)                       //监听socket构造函数
{
    int fd=Socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(":");
        exit(-1);
    }
    addr_=InetAddr(_ip,_port);                  
}
Socket::Socket()                                                //客户端socket构造函数
{
    fd_=_fd;
    addr_=InetAddr();
}
Socket::~Socket()                                               //析构函数
{

}
void SetNonBlock();                                             //非阻塞
{
    int falgs=fcntl(fd_,F_GETFL,0);
    flags|=O_NONBLOCK;
    if(fcntl(fd_,F_SETFL,flags)==-1)
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(":");
        exit(-1);
    }
}
void SetReuseAddr();                                            //重用地址，避免time_wait状态
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&flag,sizoef(flag));
}
void SetReusePort();                                            //重用端口，避免惊群效应，在reactor中意义不大
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&flag,sizoef(flag));
}
void SetTcpNoDelay();                                           //避免小数据包延迟
{
    int flag=1;
    setsockopt(fd_,IPPROTO_TCP,TCP_NODELAY,&flag,sizoef(flag));
}
void SetKeepAlive();                                            //避免僵尸进程
{
    int flag=1;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&flag,sizoef(flag));
}