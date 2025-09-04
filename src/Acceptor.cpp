#include"Acceptor.h"
Acceptor::Acceptor(std::string _ip,uint16_t _port,EventLoop* _eventLoop)
{
    servSocket=new Socket(_ip,_port);                       //初始化服务端socket
    servSocket->SetNonBlock();                              //设置非阻塞套接字
    servSocket->SetReuseAddr();                             //惊群效应
    servSocket->SetReusePort();                             //time_wait
    servSocket->SetTcpNoDelay();                            //小数据包延迟
    servSocket->SetKeepAlive();                             //僵尸连接

    servSocket->bind(InetAddr(_ip,_port));                  //绑定
    servSocket->listen(128);                                //监听（128）

    eventLoop_=_eventLoop;                                  //已有eventLoop
    servChannel=new Channel(servSocket->fd(),eventLoop_);    //服务端Channel
    servChannel->EnableReading();                           //注册读事件
    servChannel->SetHandleReadEvent(std::bind(&Acceptor::NewConnection,this));          //设置channle的读事件处理
}
Acceptor::~Acceptor()                                   
{
    delete servSocket;                                      //delete servSocket
    delete servChannel;                                     //delete servChannel 
}
void Acceptor::SetNewConnectionCB(std::function<void(Socket *clieSocket)> _fun)
{
    newConnectionCB=_fun;
}
void Acceptor::NewConnection()
{
    InetAddr clieAddr;                                      //客户端地址
    int clie_fd=servSocket->accept(clieAddr);                //通过accept获取客户端fd
    Socket *clieSocket=new Socket(clie_fd,clieAddr.ip(),clieAddr.port());////////////////////////////////////////////////////////////////////////////////////////客户端socket,在Connectoin中delete，这并不好
    clieSocket->SetKeepAlive();                             //僵尸连接
    clieSocket->SetNonBlock();                              //非阻塞
    clieSocket->SetReuseAddr();                             //time_wait
    clieSocket->SetReusePort();                             //将群效应，意义不大
    clieSocket->SetTcpNoDelay();                            //小包延迟

    newConnectionCB(clieSocket);                            //将客户端socket传递至下游
}
