#pragma once
#include<string>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<cstdint>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include"InetAddr.h"
class Socket
{
private:
    int fd_;                                    //文件描述符
    std::string ip_;                                 //ip地址
    uint16_t port_;                             //端口
public:
    Socket(std::string _ip, uint16_t _port);    //服务端socket构造函数（ip和port）
    Socket(int _fd,InetAddr clieAddr);          //客户端socket构造函数
    Socket();                                   //默认构造函数
    ~Socket();                                  //析构函数

    void SetNonBlock();                         //非阻塞
    void SetReuseAddr();                        //重用地址，避免time_wait状态
    void SetReusePort();                        //重用端口，避免惊群效应，在reactor中意义不大
    void SetTcpNoDelay();                       //避免小数据包延迟
    void SetKeepAlive();                        //避免僵尸进程

    void bind(InetAddr _addr);                  //绑定ip和端口
    void listen(int len);                       //设置监听
    int accept(InetAddr clieAddr_);             //受理连接

    int fd();                                   //fd接口
};