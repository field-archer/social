#pragma once
#include<vector>
#include<map>
#include<string.h>
#include"Acceptor.h"
#include"EventLoop.h"
#include"Connectio.h"

class EventLoop;
class Acceptor;

class TcpServer
{
private:
    Acceptor *acceptor_;                        //用于接收新连接，产生socket
    EventLoop *eventLoop_;                      //事件循环
    std::map<int,Connection*>connections;       //map:fd-Connection管理生命周期
    std::vector<std::string> messages;         //房间历史消息
public:
    TcpServer(std::string _ip,uint16_t _port);  //构造函数
    ~TcpServer();                               //析构函数：acceptor eventLoop_ connection们

    void Start();                               //处理事件
 
    void NewConnection(Socket *clieSocket);     //处理新连接，将fd交给Connection，处理客户端ip和port
    // void LogIn(int _fd,char *_name);            //用户登录，从usersUnLog转移至usersLogIn
    void LogOut(int _fd);                              //处理登出
    void Close(int _fd);                //根据fd关闭Connection，map中也删除
    void Message(Connection *_connection,std::string message);                             //处理用户消息
};