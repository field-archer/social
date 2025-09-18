#include"TcpServer.h"

TcpServer::TcpServer(std::string ip,uint16_t port,int _threadNum)
            :eventLoop_(new EventLoop(true)),
            threadNum_(_threadNum),
            threadPool(threadNum_,"IO线程"),
            acceptor_(ip,port,eventLoop_.get())
{
    acceptor_.SetNewConnectionCB(bind(&TcpServer::NewConnection,this,std::placeholders::_1));      //绑定fd传递函数
    for(int i=0;i<_threadNum;i++)
    {
        subLoops.emplace_back(new EventLoop(false,5,10));
        subLoops[i]->SetTimeOutCB(std::bind(&TcpServer::ConnectionTimeOut,this,std::placeholders::_1));
        threadPool.AddTask(std::bind(&EventLoop::run,subLoops[i].get()));           //IO线程
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
    int fd=_clieSocket->fd();                       //提前获取fd，因为下一句new Connection传参时参数计算顺序不定，可能先算move导致丢失_clieSocket的
    // auto tmp=new Connection(subLoops[fd%threadNum_],std::move(_clieSocket));
    spConnection newConnection(new Connection(subLoops[fd%threadNum_].get(),std::move(_clieSocket)));                     //将fd分发给Connection类
    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections[newConnection->fd()]=newConnection;   
    }                                                        //存入map便于管理
    subLoops[fd%threadNum_]->newConnection(newConnection);
    newConnection->SetHandleMessageEvent(std::bind(&TcpServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));  //绑定消息处理函数
    newConnection->SetHandleCloseEvent(std::bind(&TcpServer::Close,this,std::placeholders::_1));                        //绑定关闭连接的回调函数
}

//关闭连接
void TcpServer::Close(int _fd)
{
    CloseCB(_fd);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections.erase(_fd);                         //从map中移除
    }
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
void TcpServer::ConnectionTimeOut(int _fd)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections.erase(_fd);
    }
    // std::cout << "Use count: " << connections.use_count() << "\n"
}
void TcpServer::Stop()
{
    eventLoop_->Stop();
    printf("主事件循环停止\n");
    for(int i=0;i<threadNum_;i++)
    {
        subLoops[i]->Stop();
    }
    printf("从事件循环停止\n");
    threadPool.Stop();
    printf("IO线程停止\n");
}

