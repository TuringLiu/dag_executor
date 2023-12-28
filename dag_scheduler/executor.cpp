#include "executor.h"
#include <iostream>

Executor* Executor::instance_ = new Executor();

Executor::Executor()
{
    thread_pool_ = new ThreadPool(4, 10);
}

Executor* Executor::get_instance()
{
    return Executor::instance_;
}

void Executor::works_over()
{
    instance_->thread_pool_->stop();
}

void Executor::add_task(std::function<void()>&& f)
{
    // put f into task queue
    thread_pool_->add_task(std::forward<std::function<void()>>(f));
}
