#pragma once
#include<iostream>
#include<functional>
#include<sys/socket.h>
#include<unistd.h>
#include <sstream>
#include"EventLoop.h"               //eventLoop和epoll均只有一个，用eventloop替代epoll，以便在tcpserver中使用eventLoop替代epoll
class Epoll;
class EventLoop;


class Channel
{
private:
    int fd_;                                //fd由Channel管理
    EventLoop *eventLoop_;                  //eventLoop不属于channel，无需析构
    uint32_t Tevent_;                       //关注的事件
    uint32_t Revent_;                       //实际发生的事件，需手动维护，便于事件处理
    bool inEpoll_=false;                    //是否在epoll_，需手动维护，用于区分update时的mod和add
public:
    Channel(int _fd,EventLoop *_eventLoop); //构造函数
    ~Channel();                             //析构函数：fd


    void HandleEvent();                                                 //处理事件
    // //登录
    // std::function<void(std::string _name)>HandleLogInEvent;                 //处理用户登录事件的回调函数，通知到Connectioon层
    // void SetHandleLogInEvent(std::function<void(std::string _name)>);       //设置处理用户登录的回调函数
    // //登出
    // std::function<void()>HandleLogOutEvent;                                 //处理用户登出事件的回调函数，通知到TcpServer层
    // void SetHandleLogOutEvent(std::function<void()>);                       //设置处理用户登出的回调函数
    // //消息
    // std::function<void(std::string)> HandleMessageEvent;                    //处理发送消息事件的回调函数，通知到TcpServer层
    // void SetHandleMessageEvent(std::function<void(std::string)>);           //设置处理消息事件的回调函数
    // void Send(std::string name,std::string message);                                         //发送消息
    // void MessageToHttp(std::string& message);                               //将消息转化成http格式
    //断开连接
    std::function<void(int)>HandleCloseEventCB;                               //处理断开连接事件的回调函数，通知到TcpServer层
    void SetHandleCloseEvent(std::function<void(int)>);                     //设置处理断开连接的回调函数
    std::function<void()>HandleReadEventCB;                                   //读事件回调函数
    void SetHandleReadEvent(std::function<void()>);                         //设置读回调函数
    std::function<void()>HandleWriteEventCB;                                //写回调函数
    void SetHandleWriteEvent(std::function<void()>);                        //设置写回调函数


    void SetRevent(uint32_t _event);        //设置发生的事件
    void SetInEpoll();                      //设置inEpoll
    void EnableReading();                   //注册读事件
    void DisableReading();                  //注销读事件
    void EnableWriting();                   //注册写事件
    void DisableWriting();                  //注销写事件
    void UseEt();                           //使用边缘触发

    bool inEpoll();                         //返回inEpoll
    uint32_t Tevent();                      //返回Tevent
    uint32_t Revent();                      //返回Revent
    int fd();                               //返回fd
    
};