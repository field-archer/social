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
void TcpServer::NewConnection(Socket *_clieSocket)  //Connection 两个Usermap 
{
    Connection *newConnection=new Connection(_clieSocket->fd(),eventLoop_,_clieSocket);                     //将fd分发给Connection类
    printf("%d(%s:%d)已加入房间\n",_clieSocket->fd(),_clieSocket->ip().c_str(),_clieSocket->port());         //打印客户端信息
    connections[_clieSocket->fd()]=newConnection;                                                           //存入map便于管理
    newConnection->SetHandleLogOutEvent(std::bind(&TcpServer::LogOut,this,_clieSocket->fd()));                         //绑定登出函数
    newConnection->SetHandleMessageEvent(std::bind(&TcpServer::Message,this,newConnection,std::placeholders::_1));  //绑定消息处理函数
    newConnection->SetHandleCloseEvent(std::bind(&TcpServer::Close,this,_clieSocket->fd()));                        //绑定关闭连接的回调函数
    
}

void TcpServer::LogOut(int _fd)
{
    printf("%d关闭连接\n",_fd);
    delete connections[_fd];                        //关闭Connection（含Channel不含Socket）                
    connections.erase(_fd);                         //从map中移除
}
//关闭连接
void TcpServer::Close(int _fd)
{
    printf("%d关闭连接\n",_fd);
    delete connections[_fd];                        //关闭Connection（含Channel不含Socket）                
    connections.erase(_fd);                         //从map中移除
}
//处理用户消息
void TcpServer::Message(Connection *_connection,std::string message)                   
{
    printf("发送消息了!\n");
    messages.push_back(message);
    for(auto& pair:connections)
    {
        auto& connection=pair.second;
        // if(connection==_connection)continue;
        connection->send(message);
    }
}

