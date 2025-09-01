#pragma once
#include<string.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include"InetAddr.h"
class Socket
{
private:
    InetAddr addr_;                             //服务器地址（监听socket）/客户地址（客户端socket）
    int fd_;                                    //文件描述符
public:
    Socket(string _ip,uint16_t _port);          //监听socket构造函数（ip和port）
    Socket();                                   //客户端socket构造函数
    ~Socket();                                  //析构函数

    void SetNonBlock();                         //非阻塞
    void SetReuseAddr();                        //重用地址，避免time_wait状态
    void SetReusePort();                        //重用端口，避免惊群效应，在reactor中意义不大
    void SetTcpNoDelay();                       //避免小数据包延迟
    void SetKeepAlive();                        //避免僵尸进程
};