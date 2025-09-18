#pragma once
#include<iostream>
#include<string>
#include<functional>
#include<memory>
#include<queue>
#include<atomic>
#include<memory>
#include<functional>
#include<mutex>
#include<sys/eventfd.h>
#include"Channel.h"
#include"EventLoop.h"
#include"Socket.h"
#include"Buffer.h"
#include"TimeStamp.h"
class Channel;
class EventLoop;
class Socket;
class Buffer;
class Connection;
class TimeStamp;
using spConnection=std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* eventLoop_;
    std::unique_ptr<Channel> clieChannel_;                      //客户端Channel
    std::unique_ptr<Socket> clieSocket_;                        //客户端socket，暂时用Connection管理，实际Connection类不应该有Socket成员
    Buffer inputBuffer;                         //接收缓冲区
    Buffer outputBuffer;                        //发送缓冲区
    std::function<void(int)> HandleCloseCB;                            //处理断开连接的回调函数
    std::function<void(spConnection,std::string _message)> HandleMessageCB;                             //处理读回调函数，通知至TcpServer，接收消息并析出消息正文
    std::atomic<bool> close_;
    TimeStamp lastTime_;
public:
    Connection(EventLoop* _eventLoop,std::unique_ptr<Socket> _clieSocket);  //根据fd和eventLoop产生Channel
    ~Connection();                                                  //析构：clieChannel 
    
    void HandleReadEvent();                                                                 //调用读回调函数
    void SetHandleMessageEvent(std::function<void(spConnection,std::string _message)>);                  //设置读回调函数，TcpServer的处理读函数
    void HandleCloseEvent();                                                                //处理断开连接的回调函数
    void SetHandleCloseEvent(std::function<void(int)> _fun);                                   //设置断开连接的回调函数
    void HandleWriteEvent();                                                                //处理写事件

    int fd();
    void send(std::string message);                                                         //注册读事件，写入outputBuffer，实则不发送
    // void sendInIOThread(std::string message);                                                                  //在IO线程（事件循环线程）中发送
    void sendInIOThread(std::shared_ptr<std::string> message);    
    bool timeOut(time_t _now,int sec);//传入现在事件和秒数，判断是否超时
};