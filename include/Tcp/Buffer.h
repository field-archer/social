#pragma once
#include<string>
#include<iostream>
#include<string.h>
#include<cstdint>
#include<arpa/inet.h>

class Buffer
{
private:
    std::string buff_;
    const int sep_;             //分隔符：0---无，1---报文头部4字节表长度，2------\r\n
public:
    Buffer(int _sep=2);
    ~Buffer();
    void Append(const char *data,size_t size);
    void AppendWithHead(const char *data,size_t size);  //头部+正文入buff_
    void erase(ssize_t pos,int len);            //删除buff_内容
    size_t size();
    const char *data();
    void clear();
    bool getMessage(std::string& message);  //从buff_中得到消息正文
};