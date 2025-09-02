#include"TcpServer.h"

TcpServer::TcpServer(std::string ip,uint16_t port)
{
    eventLoop_=new EventLoop();
    acceptor_=new Acceptor(ip,port,eventLoop_);
    acceptor_->SetNewConnectionCB(bind(&TcpServer::NewConnection,this,std::placeholders::_1));

}
TcpServer::~TcpServer()
{
    delete acceptor_;
    delete eventLoop_;
}   
void TcpServer::Start()
{
    eventLoop_->run();
}
void TcpServer::NewConnection(Socket *clieSocket)
{
    Connection *newConnection=new Connection(clieSocket->fd(),eventLoop_);

}

