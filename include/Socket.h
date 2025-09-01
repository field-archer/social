#pragma once
#include<string.h>
#include"InetAddr.h"
class Socket
{
private:
    InetAddr addr_;                             //服务器地址（监听socket）/客户地址（客户端socket）
public:
    Socket(string _ip,uint16_t _port);          //监听socket构造函数（ip和port）
    Socket();                                   //客户端socket构造函数
    ~Socket();                                  //析构函数
};