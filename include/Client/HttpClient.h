#pragma once

#include"TcpClient.h"
#include"HttpRequest.h"
#include"HttpResponse.h"

class HttpClient
{
private:
    TcpClient tcpClient_;

    HttpRequest httpRequest_;
    HttpResponse httpResponse_;
    void Recv(std::string& _message);               //接收数据
    void Send(const std::string& _message);         //发送数据
public:
    HttpClient(const std::string& _ip,uint16_t _port);

};