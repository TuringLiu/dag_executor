#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <queue>
#include <vector>
#include <atomic>
// todo: support return-value ways [future instance]
// todo：目前的有界任务队列，任务溢出时，需放至redis缓存中
// todo: 引入信号量机制，当任务队列超过1s无任务执行时，就切换至阻塞态
// todo: 引入future，等待子线程执行完毕【此时主线程被阻塞】

class Worker;

class ThreadPool
{
    friend class Worker;
public:
    ThreadPool() = delete;
    ThreadPool(const int& worker_num, const int& tasks_size);
    ~ThreadPool();

    void add_task(std::function<void()>);
    void stop();

private:
    void run_();
    void thread_over_();

private:
    int worker_num_;
    int tasks_size_;
    std::vector<Worker*> workers_;
    std::vector<std::thread> threads_;

    std::atomic<int> idx_{0};
    std::mutex mtx_;

    std::atomic<bool> stop_{false};
    std::atomic<int> stop_num_{0};
    std::condition_variable stop_cond_;

};

class Worker
{
    friend class ThreadPool;
public:
    Worker() = delete;
    Worker(const int& tasks_size, const int& id);

    bool add_task(std::function<void()>);
    void work_();

private:
    void work_steal_();

private:
    std::mutex task_mtx_;
    std::condition_variable task_cond_;
    std::queue<std::function<void()>> tasks_queue_;
    
    int tasks_size_;
    int worker_id_;

    ThreadPool* thread_pool_;
};
