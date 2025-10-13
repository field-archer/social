#pragma once
#include<vector>
#include<map>
#include<memory>
#include<queue> 
#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/eventfd.h>
#include<sys/timerfd.h>
#include<mutex>
#include<atomic>
#include"Epoll.h"
#include"Channel.h"
#include"Connection.h"
class Channel;
class Epoll;
class Connection;
using spConnection=std::shared_ptr<Connection>;

class EventLoop
{
private:
    int timeval_;                                                       //定时器周期
    int timeout_;                                                       //超时界限，每隔timeval_处理超过timeout_无信息交换的连接
    std::unique_ptr<Epoll> epoll_;                                      //生命周期和EventLoop一样，析构
    pid_t threadID_;                                                    //线程ID
    std::mutex mutex_;
    int wakefd_;
    std::unique_ptr<Channel> wakeChannel_;
    std::queue<std::function<void()>> taskQueue_;
    int tfd_;
    std::unique_ptr<Channel> timeChannel_;                              //定时器Channel
    bool mainLoop_;                                                     //是否是主事件循环（仅受理新连接）
    std::map<int,spConnection> connections_;                            //存储该事件循环的连接（仅子EventLoop有）
    std::function<void(int)> timeOutCB;                                 //处理超时的回调函数
    std::mutex mmutex_;
    std::atomic<bool> stop_;
public:
    EventLoop(bool _mainLoop,int _timeval=30,int _timeut=300);                          //构造函数
    ~EventLoop();                                       //析构函数：epoll_
    
    void run();                                         //事件循环
    void UpdateChannel(Channel *ch);                    //调用Epoll的update
    void removeChannel(Channel *ch);                    //清楚Channel
    
    bool isLoopThread();                                //判断当前线程是否为事件循环线程
    
    void wakeUp();
    void HandleWake();
    void addTaskLoop(std::function<void()>);
    
    void HandleAlarm();
    void newConnection(spConnection _newConnection);
    
    void SetTimeOutCB(std::function<void(int)>);

    void Stop();

    void DelConnection(int fd);                         //删除map中fd对应Connection
};