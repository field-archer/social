#pragma once
#include<cstdio>
#include<vector>
#include<queue>
#include<thread>
#include<functional>
#include<mutex>
#include<condition_variable>
#include<atomic>

class ThreadPool
{
private:
    std::vector<std::thread>threads_;               //线程s
    std::queue<std::function<void()>>taskQueue_;    //任务队列
    std::mutex mutex_;                              //锁
    std::condition_variable cv_;                    //条件变量
    std::atomic<bool> stop_;
public:
    ThreadPool(int _num);
    ~ThreadPool();
    void AddTask(std::function<void()>);
};