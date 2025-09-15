#include"ThreadPool.h"
/*
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
*/
ThreadPool::ThreadPool(int _num):stop_(false)
{
    for(int i=0;i<_num;i++)
    {
        threads_.emplace_back([this]
        {
            while(stop_==false)
            {
                std::function<void()>task;
                {
                    std::unique_lock<std::mutex>lock(mutex_);

                    cv_.wait(lock,[this]
                    {
                        return stop_==true||!taskQueue_.empty();    //有任务/关闭线程池 会唤醒线程
                    });

                    if(stop_==true&&taskQueue_.empty())break;       //stop且当前无任务退出，有任务则执行完当前任务再退出

                    task=std::move(taskQueue_.front());
                    taskQueue_.pop();
                }
                task();
            }
        });
    }
}
ThreadPool::~ThreadPool()
{
    stop_=true;
    cv_.notify_all();
    for(auto& th:threads_)
    {
        th.join();
    }
}
void ThreadPool::AddTask(std::function<void()> _fun)
{
    
    {           //锁作用域
        std::lock_guard<std::mutex> lock(mutex_);
        taskQueue_.push(_fun);
    }
    cv_.notify_one();   //唤醒一个线程处理任务
}