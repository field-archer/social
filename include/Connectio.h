#pragma once
#include"Channel.h"
#include"EventLoop.h"
class Channel;
class EventLoop;

class Connection
{
private:
    int fd_;
    Channel* clieChannel_;
    EventLoop* eventLoop_;
public:
    Connection(int _fd,EventLoop *_eventLoop);
    ~Connection();
};