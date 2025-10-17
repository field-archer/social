#include"HttpClient.h"
HttpClient::HttpClient(const std::string& _ip,uint16_t _port):tcpClient_(_ip,_port)
{
    
}
//接收数据
void HttpClient::Recv(std::string& _message)
{
    tcpClient_.Recv(_message);
}
//发送数据
void HttpClient::Send(const std::string& _message)
{
    tcpClient_.Send(_message);
}