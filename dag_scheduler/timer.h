#include <queue>
#include <vector>
#include <functional>
#include <mutex>
#include <chrono>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <memory>
#include "util/basic_function.h"
#include "thread_pool/thread_pool.h"


class Timer
{
    friend struct TimerManager;
public:
    Timer(long long time, std::function<void()> func) : time_(time), func_(func) {}
    bool operator<(const Timer& other) const
    {       
        return time_ < other.time_;
    }
private:
    long long time_; // absolute time
    std::function<void()> func_;
};

class TimerManager
{
public:
    TimerManager(std::shared_ptr<ThreadPool> thread_pool);

    // relative_time: millisecond
    void add(long long relative_time, std::function<void()> func);
    void execute_timeout();

private:
    struct TimerCmp
    {
        bool operator()(const Timer& a, const Timer& b)
        {
            return a.time_ > b.time_;
        }
    };


    std::priority_queue<Timer, std::vector<Timer>, TimerCmp> timers_;
    std::mutex mutex_;
    std::shared_ptr<ThreadPool> thread_pool_;
};

void TimerManager::add(long long relative_time, std::function<void()> func)
{ 
    std::lock_guard<std::mutex> lock(mutex_);
    timers_.push(Timer(relative_time + relative_time_count(), func));
}

void TimerManager::execute_timeout()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while(!timers_.empty())
    {
        if(timers_.top().time_ <= relative_time_count())
        {
            thread_pool_->add_task(timers_.top().func_);
            timers_.pop();
        }
        else break;
    }

}

TimerManager::TimerManager(std::shared_ptr<ThreadPool> thread_pool)
{
    thread_pool_ = thread_pool;

    int epollFd = epoll_create1(0);  // 创建 epoll 实例
    int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

    struct epoll_event event;
    event.events = EPOLLIN;  // 监听可读事件
    event.data.fd = timerFd;  // 监听定时器文件描述符

    epoll_ctl(epollFd, EPOLL_CTL_ADD, timerFd, &event);

    
    struct itimerspec timerSpec;
    timerSpec.it_value.tv_sec = 5;  // 初始延时 5 秒
    timerSpec.it_value.tv_nsec = 0;
    timerSpec.it_interval.tv_sec = 1;  // 定时周期为 1 秒
    timerSpec.it_interval.tv_nsec = 0;

    timerfd_settime(timerFd, 0, &timerSpec, nullptr);
    constexpr int maxEvents = 10;
    struct epoll_event events[maxEvents];

    while (true) {
        int numEvents = epoll_wait(epollFd, events, maxEvents, -1);
        if (numEvents == -1) {
            std::cerr << "epoll_wait error." << std::endl;
            close(timerFd);
            close(epollFd);
        }

        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == timerFd) {
                std::cout << "Timer event received." << std::endl;
                // 在这里处理定时器事件
                uint64_t expirations;
                read(timerFd, &expirations, sizeof(expirations));  // 读取过期次数，防止定时器溢出
            }
        }
    }

    close(timerFd);
    close(epollFd);
}