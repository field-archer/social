#include"TcpServer.h"

TcpServer::TcpServer(std::string ip,uint16_t port)
{
    eventLoop_=new EventLoop();                                                                     //初始化eventLoop，暂时只有一个
    acceptor_=new Acceptor(ip,port,eventLoop_);                                                     //初始化acceptor
    acceptor_->SetNewConnectionCB(bind(&TcpServer::NewConnection,this,std::placeholders::_1));      //绑定fd传递函数
}
TcpServer::~TcpServer()
{
    for(auto &pair:connections)
    {
        delete pair.second;                         //delete 所有连接
    }
    delete acceptor_;                               //delete acceptor
    delete eventLoop_;                              //delete 事件循环
}   
void TcpServer::Start()
{
    eventLoop_->run();                              //事件循环
}
void TcpServer::NewConnection(Socket *_clieSocket)
{
    Connection *newConnection=new Connection(_clieSocket->fd(),eventLoop_,_clieSocket);                     //将fd分发给Connection类
    printf("%d(%s:%d)已加入连接\n",_clieSocket->fd(),_clieSocket->ip(),_clieSocket->port());
    connections[_clieSocket->fd()]=newConnection;                                                           //存入map便于管理
    newConnection->SetChannelCloseCB(std::bind(&TcpServer::DelConnection,this,std::placeholders::_1));      //绑定关闭连接函数
}
void TcpServer::DelConnection(int _fd)
{
    printf("%d关闭连接\n",_fd);
    delete connections[_fd];                        //关闭Connection（含Channel不含Socket）                
    connections.erase(_fd);                         //从map中移除
}

