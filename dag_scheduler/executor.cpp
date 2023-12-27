#include "executor.h"

Executor* Executor::instance_ = new Executor();


Executor* Executor::get_instance()
{
    return Executor::instance_;
}

void Executor::add_task(std::function<void()> f)
{
    // put f into task queue
    f();
}
