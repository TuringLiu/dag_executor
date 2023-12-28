#include <functional>
#include "thread_pool/thread_pool.h"

// singleton 
class Executor
{
public:
    void add_task(std::function<void()>&& f);
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
