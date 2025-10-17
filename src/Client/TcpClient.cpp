#include"TcpClient.h"

TcpClient::TcpClient(const std::string& _ip,uint16_t _port):
    sockfd_(socket(AF_INET,SOCK_STREAM,0)),ip_(_ip),port_(_port),addr_(ip_,port_)
{
    try
    {
        if(sockfd_==-1)throw std::runtime_error("客户端socket初始化出错");

        SetNonBlock();

        Connect();
    }
    catch(const std::exception& e)
    {//抛出异常，不补充
        throw std::runtime_error(e.what());
   }
    
    
}
//连接服务端
void TcpClient::Connect()
{
    // socklen_t addrLen=sizeof(addr_.addr());
    if(connect(sockfd_,addr_.addr(),sizeof(addr_.addr()))==-1)throw std::runtime_error("连接服务端出错");
}
void TcpClient::SetNonBlock()                                              //非阻塞
{
    int flags=fcntl(sockfd_,F_GETFL,0);
    flags|=O_NONBLOCK;
    if(fcntl(sockfd_,F_SETFL,flags)==-1)throw std::runtime_error("设置非阻塞套接字失败");
}
//开始于服务端通讯，一发一收
// void TcpClient::Start()
// {

//     while(true)
//     {

//     }
// }
//接收数据
void TcpClient::Recv(std::string& _message)
{
    _message.clear();
    int maxsize=1024;
    char buffer[maxsize]="";
    while(true)
    {
        int recvn=recv(sockfd_,buffer,maxsize,0);
        if(recvn>0)//继续接收
        {
            _message.append(buffer,recvn);
        }else if(recvn==-1&&(errno==EWOULDBLOCK||errno==EAGAIN))//接收完毕
        {
            break;
        }else if(recvn==-1&&errno==EINTR)//信号中断，重试
        {
            continue;
        }else if(recvn==0)//对端关闭
        {
            break;
        }
    }
}
//发送数据
void TcpClient::Send(const std::string& _message)
{
    int haveSend=0;     //已发送字节数
    while(true)
    {
        ssize_t sendn=send(sockfd_,_message.c_str()+haveSend,_message.size()-haveSend,0);
        if(sendn>0)//继续发送
        {
            haveSend+=sendn;
        }else if(sendn==-1&&(errno==EWOULDBLOCK||errno==EAGAIN))//发送完毕
        {
            break;
        }else if(sendn==-1&&errno==EINTR)//信号中断，重试
        {
            continue;
        }else if(sendn==0)//对端关闭
        {
            break;
        }
    }
}