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
Buffer::Buffer(int _sep):sep_(_sep)
{

}
Buffer::~Buffer()
{

}
void Buffer::Append(const char *data,size_t size)
{
    std::string str(data,size);
    std::cout<<"发送的消息为"<<str<<"\n";
    buff_.append(data,size);
}
//头部+正文入buff_
void Buffer::AppendWithHead(const char *data,size_t size)
{
    if(sep_==0)
    {
        buff_.append(data,size);
    }else if(sep_==1)
    {
        uint32_t net_len=htonl(size);
        buff_.append((char*)&net_len,4);
        buff_.append(data,size);
    }else if(sep_==2)
    {
        //\r\n格式
    }
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
//从buff_中得到消息正文
bool Buffer::getMessage(std::string& _message)
{
    if(buff_.empty()){printf("buff为空\n");return false;}
    if(sep_==0)
    {
        _message=buff_;
        buff_.clear();
    }else if(sep_==1)
    {
        //接收完毕
        uint32_t net_len;
        memcpy(&net_len,buff_.data(),4);
        int len=ntohl(net_len);
        //不足一份报文，需后续处理
        if(buff_.size()<len+4)
        {
            printf("报文长度不够\n");
            // printf("报文为%s###\n",_message.c_str());
            return false;
        }
        _message.append(buff_.data()+4,len);
        // std::string message(buff_.data()+4,len);
        buff_.erase(0,len+4);
        
    }else if(sep_==2)
    {
        _message.append(buff_.data(),size());
        // printf("消息=%s###\n",buff_.c_str());
        buff_.clear();
    }
    return true;
}