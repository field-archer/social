#include"Buffer.h"
/*class Buffer
{
private:
    std::string buff_;
public:
    Buffer();
    ~Buffer();
    void Append(const char *data,size_t size);
    size_t size();
    const char *data();
    void clear();
};
*/
Buffer::Buffer()
{

}
Buffer::~Buffer()
{

}
void Buffer::Append(const char *data,size_t size)
{
    buff_.append(data,size);
}
//头部+正文入buff_
void Buffer::AppendWithHead(const char *data,size_t size)
{
    // int len=size;
    uint32_t net_len=htonl(size);
    buff_.append((char*)&net_len,4);
    buff_.append(data,size);
}
void Buffer::erase(ssize_t pos,int len)
{
    buff_.erase(pos,len);
}
size_t Buffer::size()
{
    return buff_.size();
}
const char* Buffer::data()
{
    return buff_.data();
}
void Buffer::clear()
{
    buff_.clear();
}