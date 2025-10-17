#include"Acceptor.h"
Acceptor::Acceptor(std::string _ip,uint16_t _port,EventLoop* _eventLoop)
                :eventLoop_(_eventLoop),servSocket(_ip,_port),servChannel(servSocket.fd(),eventLoop_)
{
    servSocket.SetNonBlock();                              //设置非阻塞套接字
    servSocket.SetReuseAddr();                             //惊群效应
    servSocket.SetReusePort();                             //time_wait
    servSocket.SetTcpNoDelay();                            //小数据包延迟
    servSocket.SetKeepAlive();                             //僵尸连接

    servSocket.bind(InetAddr("0.0.0.0",_port));                  //绑定
    servSocket.listen(128);                                //监听（128）

    servChannel.EnableReading();                           //注册读事件
    servChannel.SetHandleReadEvent(std::bind(&Acceptor::NewConnection,this));          //设置channle的读事件处理
}
Acceptor::~Acceptor()                                   
{
    // delete servChannel;                                     //delete servChannel 
}
void Acceptor::SetNewConnectionCB(std::function<void(std::unique_ptr<Socket> clieSocket)> _fun)
{
    newConnectionCB=_fun;
}
void Acceptor::NewConnection()
{
    InetAddr clieAddr;                                      //客户端地址
    int clie_fd=servSocket.accept(clieAddr);                //通过accept获取客户端fd
    std::unique_ptr<Socket> clieSocket(new Socket(clie_fd,clieAddr.ip(),clieAddr.port()));
    clieSocket->SetKeepAlive();                             //僵尸连接
    clieSocket->SetNonBlock();                              //非阻塞
    clieSocket->SetReuseAddr();                             //time_wait
    clieSocket->SetReusePort();                             //将群效应，意义不大
    clieSocket->SetTcpNoDelay();                            //小包延迟
    newConnectionCB(std::move(clieSocket));                 //将客户端socket传递至下游
}
