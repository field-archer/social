#pragma once
#include"TcpServer.h"
#include"ThreadPool.h"
#include<string>
#include"HttpRequest.h"
#include"HttpConnection.h"
#include"HttpResponse.h"
#include"MYSQLConnectionPool.h"
#include"HttpContext.h"
class TcpServer;
class ThreadPool;
using upContext=std::unique_ptr<HttpContext>;//HttpContext指针
using httpHandler=std::function<void(upContext)>;
// using upMYSQLPool=std::unique_ptr<MYSQLConnectionPool>;

class HttpServer
{
private:
    std::unordered_map<std::string,httpHandler>GetHandler_;             //get方法处理
    std::unordered_map<std::string,httpHandler>PostHandler_;            //post方法处理

    TcpServer tcpServer_;                                               //tcpServer层
    // ThreadPool workThreadPool_;                                         //工作线程，在httpServer没有用到，只有初始化和停止

    // upMYSQLPool mysqlPool_;                                             //mysql连接池

    //底层
    void Close(int _fd);                                                                //根据fd关闭Connection，map中也删除

    void HandleMessage(spConnection _connection,std::string _message);                  //处理消息

    //业务层
    void NewConnection(Socket *clieSocket);                                             //处理新连接，将fd交给Connection，处理客户端ip和port

    void SetGetHandler();                                                               //设置Get处理
    void SetPostHandler();                                                              //设置post处理

    
public:
    HttpServer(const std::string& _ip,uint16_t _port,int _eventLoopNum,int _workNum,
                //mysql配置
                int _mysqlPort,const std::string& _user,const std::string& _passwd,//端口（3306），用户，密码
                const std::string& _dataBase,size_t _maxSize,size_t _minSize,//数据库，最大总连接数，最小总连接数/最大空闲连接数
                int _idleCheckInterval,int _connectionTimeOut);//空闲连接检查间隔，空闲连接超时时间
    HttpServer(const std::string& _ip,uint16_t _port,int _eventLoopNum);
    ~HttpServer();

    void Start();                                                                       //处理事件
    void Stop();                                                                        //停止线程池和tcpServer

    void SetSignUp(std::function<void(upContext)> _fun);                                         //设置用户注册回调函数
    void SetLogIn(std::function<void(upContext)> _fun);                                          //设置用户登录回调函数


};