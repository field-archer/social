#pragma once
#include<cstdio>
#include<vector>
#include<string>
#include<queue>
#include<thread>
#include<functional>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<sys/syscall.h>
#include<unistd.h>

class ThreadPool
{
private:
    std::string name_;
    std::vector<std::thread>threads_;               //线程s
    std::queue<std::function<void()>>taskQueue_;    //任务队列
    std::mutex mutex_;                              //锁
    std::condition_variable cv_;                    //条件变量
    std::atomic<bool> stop_;
    std::mutex mmutex_;
public:
    ThreadPool(int _num,std::string _name);
    ~ThreadPool();
    void AddTask(std::function<void()>);
    void Stop();
    size_t size();
};