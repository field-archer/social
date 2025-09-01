#include"TcpServer.h"

TcpServer::TcpServer(string ip,uint16_t port)
{
    acceptor_=new Acceptor(ip,port);

    eventLoop_=new EventLoop();
}
TcpServer::~TcpServer()
{
    delete acceptor_;
    delete eventLoop_;
}
void TcpServer::Start()
{

}
void TcpServer::SetNewConnectionCB()
{
    
}