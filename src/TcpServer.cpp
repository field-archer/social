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
void TcpServer::NewConnection(Socket *_clieSocket)  //Connection 
{
    Connection *newConnection=new Connection(_clieSocket->fd(),eventLoop_,_clieSocket);                     //将fd分发给Connection类
    connections[_clieSocket->fd()]=newConnection;                                                           //存入map便于管理
    newConnection->SetHandleMessageEvent(std::bind(&TcpServer::HandleMessage,this,newConnection,std::placeholders::_1));  //绑定消息处理函数
    newConnection->SetHandleCloseEvent(std::bind(&TcpServer::Close,this,_clieSocket->fd()));                        //绑定关闭连接的回调函数
}

//关闭连接
void TcpServer::Close(int _fd)
{
    CloseCB(_fd);
    delete connections[_fd];                        //关闭Connection（含Channel不含Socket）                
    connections.erase(_fd);                         //从map中移除
}

void TcpServer::HandleMessage(Connection *connection,std::string _message)          //处理消息
{
    MessageCB(connection,_message);
}
void TcpServer::SetNewConnectionCB(std::function<void(Socket*)> _fun)               //设置新连接业务的回调函数
{
    NewConnectionCB=_fun;
}
void TcpServer::SetCloseCB(std::function<void(int)> _fun)                           //设置关闭业务的回调函数
{
    CloseCB=_fun;
}
void TcpServer::SetMessageCB(std::function<void(Connection*,std::string)> _fun)      //设置关闭业务的回调函数
{
    MessageCB=_fun;
}

