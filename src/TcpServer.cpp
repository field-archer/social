#include"TcpServer.h"

TcpServer::TcpServer(std::string ip,uint16_t port,int _threadNum)
            :eventLoop_(new EventLoop()),
            threadNum_(_threadNum),
            threadPool(threadNum_),
            acceptor_(ip,port,eventLoop_)
{
    acceptor_.SetNewConnectionCB(bind(&TcpServer::NewConnection,this,std::placeholders::_1));      //绑定fd传递函数
    for(int i=0;i<_threadNum;i++)
    {
        subLoops.emplace_back(new EventLoop());
        threadPool.AddTask(std::bind(&EventLoop::run,subLoops[i].get()));
    }
}
TcpServer::~TcpServer()
{
    // delete acceptor_;                               //delete acceptor
}   
void TcpServer::Start()
{
    eventLoop_->run();                              //事件循环
}
void TcpServer::NewConnection(std::unique_ptr<Socket> _clieSocket)  //Connection 
{
    int fd=_clieSocket->fd();                    //提前获取fd，因为下一句new Connection传参时参数计算顺序不定，可能先算move导致丢失_clieSocket的
    auto tmp=new Connection(subLoops[fd%threadNum_],std::move(_clieSocket));
    spConnection newConnection(tmp);                     //将fd分发给Connection类
    connections[newConnection->fd()]=newConnection;                                                           //存入map便于管理
    newConnection->SetHandleMessageEvent(std::bind(&TcpServer::HandleMessage,this,newConnection,std::placeholders::_1));  //绑定消息处理函数
    newConnection->SetHandleCloseEvent(std::bind(&TcpServer::Close,this,std::placeholders::_1));                        //绑定关闭连接的回调函数
}

//关闭连接
void TcpServer::Close(int _fd)
{
    CloseCB(_fd);
    connections.erase(_fd);                         //从map中移除
}

void TcpServer::HandleMessage(spConnection connection,std::string _message)          //处理消息
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
void TcpServer::SetMessageCB(std::function<void(spConnection,std::string)> _fun)      //设置关闭业务的回调函数
{
    MessageCB=_fun;
}

