#pragma once
#include<vector>
#include<map>
#include<string.h>
#include<memory>
#include"Acceptor.h"
#include"EventLoop.h"
#include"Connectio.h"
#include"ThreadPool.h"

class ThreadPool;
class EventLoop;
class Acceptor;
using spConnection=std::shared_ptr<Connection>;

class TcpServer
{
private:
    std::unique_ptr<EventLoop> eventLoop_;                      //事件循环
    std::vector<std::unique_ptr<EventLoop>>subLoops;            //子线程事件循环，处理客户端通讯
    int threadNum_;                             //IO线程池大小
    ThreadPool IOthreadPool;                     //IO线程池
    Acceptor acceptor_;                        //用于接收新连接，产生socket
    std::mutex mutex_;
    std::map<int,spConnection>connections;       //map:fd-Connection管理生命周期
public:
    TcpServer(std::string _ip,uint16_t _port,int _threadNum=5);  //构造函数
    ~TcpServer();                               //析构函数：acceptor eventLoop_ connection们

    void Start();                               //处理事件
 
    void NewConnection(std::unique_ptr<Socket> clieSocket);     //处理新连接，将fd交给Connection，处理客户端ip和port
    void Close(int _fd);                        //根据fd关闭Connection，map中也删除
    void HandleMessage(spConnection _connection,std::string _message);   //处理消息

    std::function<void(Socket*)> NewConnectionCB;                        //新连接业务的回调函数
    void SetNewConnectionCB(std::function<void(Socket*)>);              //设置新连接业务的回调函数
    std::function<void(int)> CloseCB;                                   //关闭业务的回调函数
    void SetCloseCB(std::function<void(int)>);                          //设置关闭业务的回调函数
    std::function<void(spConnection,std::string)> MessageCB;              //关闭业务的回调函数
    void SetMessageCB(std::function<void(spConnection,std::string)>);     //设置关闭业务的回调函数

    void ConnectionTimeOut(int _fd);                                    //清理超时的连接
    void Stop();
};