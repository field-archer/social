#include"EchoTcpServer.h"
EchoTcpServer::EchoTcpServer(std::string _ip,uint16_t _port,int _eventLoopNum,int _workNum)
                :tcpServer_(_ip,_port,_eventLoopNum),workThreadPool_(_workNum,"工作线程")                   //构造函数
{
    tcpServer_.SetNewConnectionCB(std::bind(&EchoTcpServer::NewConnection,this,std::placeholders::_1));//设置新连接业务回调函数  
    tcpServer_.SetCloseCB(std::bind(&EchoTcpServer::Close,this,std::placeholders::_1));              //设置关闭业务回调函数
    tcpServer_.SetMessageCB(std::bind(&EchoTcpServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));

}                                                                                                  
EchoTcpServer::~EchoTcpServer()                                                                     //析构函数
{

}
void EchoTcpServer::Start()                                                                         //处理事件
{
    tcpServer_.Start();
}
void EchoTcpServer::NewConnection(Socket *clieSocket)                                               //处理新连接，将fd交给Connection，处理客户端ip和port
{
    printf("%d（%s:%d）已加入连接\n",clieSocket->fd(),clieSocket->ip().c_str(),clieSocket->port());
}
void EchoTcpServer::Close(int _fd)                                                                  //根据fd关闭Connection，map中也删除
{
    printf("%d关闭连接\n",_fd);
}
void EchoTcpServer::HandleMessage(spConnection _connection,std::string _message)                      //处理消息
{
    //回显
    printf("接收到：%s\n",_message.c_str());
    if(workThreadPool_.size()==0)
    {
        OnMessage(_connection,_message);
    }else 
    {
        workThreadPool_.AddTask(std::bind(&EchoTcpServer::OnMessage,this,_connection,_message));
    }
}
void EchoTcpServer::OnMessage(spConnection _connection,std::string _message)
{
    std::string tmpmessage="回复"+_message;
    _connection->send(tmpmessage);
}

void EchoTcpServer::Stop()
{
    workThreadPool_.Stop();
    printf("工作线程停止\n");
    tcpServer_.Stop();
    printf("tcpServer停止\n");
}