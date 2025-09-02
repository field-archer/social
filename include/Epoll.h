#pragma once
#include<sys/epoll.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<vector>
#include"Channel.h"
class Channel;

class Epoll
{
private:
    int epfd;                       //epoll句柄
    static const int maxsize=1024;        //////////////////////////////////////////////////////////////////1024后续修改
    epoll_event evs[maxsize];                                                                   
public:
    Epoll();                        //构造函数
    ~Epoll();                       //析构函数

    void UpdateChannel(Channel *_channel);           //更新Channel
    std::vector<Channel*>loop(int time);
};