#pragma once
#include"Channel.h"
#include"EventLoop.h"
#include"Socket.h"
class Channel;
class EventLoop;
class Socket;

class Connection
{
private:
    int fd_;                                    //fd
    Channel* clieChannel_;                      //客户端Channel
    EventLoop* eventLoop_;                      //EventLoop
    Socket* clieSocket_;                        //客户端socket，暂时用Connection管理，实际Connection类不应该有Socket成员
public:
    Connection(int _fd,EventLoop *_eventLoop,Socket *_clieSocket);  //根据fd和eventLoop产生Channel
    ~Connection();                                                  //析构：clieChannel 
    void SetChannelCloseCB(std::function<void(int)>);               //设置channel处理关闭事件的函数
};