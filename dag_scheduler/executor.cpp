#include "executor.h"

Executor* Executor::instance_ = new Executor();

void Executor::add_task(std::function<void()> f)
{
    // put f into task queue
}
