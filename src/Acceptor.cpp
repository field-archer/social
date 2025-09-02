#include"Acceptor.h"
Acceptor::Acceptor(std::string _ip,uint16_t _port,EventLoop* _eventLoop)
{
    servSocket=Socket(_ip,_port);               //初始化服务端socket
    servSocket.SetNonBlock();                   //设置非阻塞套接字
    servSocket.SetReuseAddr();                  //惊群效应
    servSocket.SetReusePort();                  //time_wait
    servSocket.SetTcpNoDelay();                 //小数据包延迟
    servSocket.SetKeepAlive();                  //僵尸连接
    printf("服务端socket已完毕\n");

    servSocket.bind(InetAddr(_ip,_port));       //绑定
    servSocket.listen(128);                     //监听（128）

    eventLoop_=_eventLoop;                       
    servChannel=new Channel(servSocket.fd(),eventLoop_);
    servChannel->EnableReading();               //注册读事件
    servChannel->SetHandleReadEvent(std::bind(&Acceptor::NewConnection,this));          //设置channle的读事件处理
}
Acceptor::~Acceptor()
{
    delete servChannel;
}
void Acceptor::SetNewConnectionCB(std::function<void(Socket *clieSocket)> _fun)
{
    newConnectionCB=_fun;
}
void Acceptor::NewConnection()
{
    InetAddr clieAddr;
    int clie_fd=servSocket.accept(clieAddr);
    Socket *clieSocket=new Socket(clie_fd,clieAddr);
    clieSocket->SetKeepAlive();
    clieSocket->SetNonBlock();
    clieSocket->SetReuseAddr();
    clieSocket->SetReusePort();
    clieSocket->SetTcpNoDelay();
    printf("客户端socket已完成\n");

    newConnectionCB(clieSocket);
}
