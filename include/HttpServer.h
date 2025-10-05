#pragma once
#include"TcpServer.h"
#include"ThreadPool.h"
#include<string>
#include"HttpRequest.h"
#include"HttpConnection.h"
#include"HttpResponse.h"
class TcpServer;
class ThreadPool;
using httpHandler=std::function<void()>;

class HttpServer
{
private:
    std::unordered_map<std::string,httpHandler>GetHandler_;

    TcpServer tcpServer_;
    ThreadPool workThreadPool_;
public:
    HttpServer(std::string _ip,uint16_t _port,int _eventLoopNum=2,int _workNum=2);
    ~HttpServer();
    void GetAdd(std::string& _path,httpHandler _fun);

    // void 
    // EchoTcpServer(std::string _ip,uint16_t _port,int _eventLoopNum=5,int workNum=5);    //构造函数
    // ~EchoTcpServer();                                                                   //析构函数
    void Start();                                                                       //处理事件
 
    void NewConnection(Socket *clieSocket);                                             //处理新连接，将fd交给Connection，处理客户端ip和port
    void Close(int _fd);                                                                //根据fd关闭Connection，map中也删除
    void HandleMessage(spConnection _connection,std::string _message);                  //处理消息
    void OnMessage(spConnection _connection,std::string _message);                      //计算消息

    void Stop();                                                                        //停止线程池和tcpServer

    void SetGetHandler();                                                               //设置Get处理
};