#include <functional>
#include "thread_pool/thread_pool.h"

// 一个线程池的接口类，调用线程池执行task

// singleton 
class Executor
{
public:
    void add_task(std::function<void()>&& f);
    std::future<int> add_task_async(std::function<void()>&& f);
    static Executor* get_instance();
    static void works_over();

    Executor(const Executor&) = delete;
    Executor& operator = (const Executor) = delete;
private:
    Executor();
    ~Executor(){};

private:
    static Executor* instance_;
    ThreadPool* thread_pool_;
};
