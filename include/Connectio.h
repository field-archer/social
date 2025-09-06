#pragma once
#include<iostream>
#include<string>
#include<functional>
#include"Channel.h"
#include"EventLoop.h"
#include"Socket.h"
#include"Buffer.h"
class Channel;
class EventLoop;
class Socket;
class Buffer;

class Connection
{
private:
    std::string  name_;                         //用户名
    Channel* clieChannel_;                      //客户端Channel
    EventLoop* eventLoop_;                      //EventLoop
    Socket* clieSocket_;                        //客户端socket，暂时用Connection管理，实际Connection类不应该有Socket成员
    Buffer inputBuffer;                         //接收缓冲区
    Buffer outputBuffer;                        //发送缓冲区
    // std::function<void()> CloseCB_;             //处理连接关闭的回调函数
    // std::function<void(char*)> LogInCB_;        //处理用户登录的回调函数
    // std::function<void(Connection*,std::string)> MessageCB_;           //处理客户端信息的回调函数
    // std::function<void()> ReadCB_;               //读事件回调函数
    std::function<void()> HandleLogOutEventCB;                      //处理登出事件的回调函数
    std::function<void(std::string message)> HandleMessageEventCB;  //处理消息事件的回调函数
    std::function<void()> HandleCloseCB;                            //处理断开连接的回调函数
public:
    Connection(int _fd,EventLoop *_eventLoop,Socket *_clieSocket);  //根据fd和eventLoop产生Channel
    ~Connection();                                                  //析构：clieChannel 
    
    // void SetCloseCB(std::function<void()>);                      //设置处理关闭连接的回调函数
    // void SetLogInCB(std::function<void(char*)>);                    //设置处理用户登陆的回调函数
    // void SetMessageCB(std::function<void(Connection*,std::string)>);                       //设置处理用户消息的回调函数
    void HandleLogInEvent(std::string _name);                                               //处理用户登录的函数
    void HandleLogOutEvent();                                                               //处理用户登出的函数
    void SetHandleLogOutEvent(std::function<void()>);                                       //设置处理登出的回调函数
    void HandleMessageEvent(std::string message);                                           //处理发送消息的函数
    void SetHandleMessageEvent(std::function<void(std::string message)>);                   //设置发送消息的回调函数
    void HandleCloseEvent();                                                                //处理断开连接的回调函数
    void SetHandleCloseEvent(std::function<void()> _fun);                                   //设置断开连接的回调函数

    void SetReadCB(std::function<void()> _ReadCB);                  //读事件回调函数
    void HandleRead();                                              //处理读事件


    void CloseCB();                                                 //处理关闭连接的函数
    void DataChange();                                              //处理正常读事件，数据收发
    int fd();
    void send(std::string message);    
    void WriteCallBack();                                           //发送数据函数，供Channel回调
};