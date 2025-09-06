#pragma once
#include<string>
class Buffer
{
private:
    std::string buff_;
public:
    Buffer();
    ~Buffer();
    void Append(const char *data,size_t size);
    void erase(ssize_t pos,int len);            //删除buff_内容
    size_t size();
    const char *data();
    void clear();
};