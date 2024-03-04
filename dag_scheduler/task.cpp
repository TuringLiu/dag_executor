#include "task.h"
#include "executor.h"
#include "scheduler.h"
#include <iostream>

void Task::execute_()
{
    pre_run_();
    work();
    post_run_();
}

void Task::pre_run_()
{
    // todo: add some log metrics init in here
    
}

void Task::post_run_()
{
    // todo: add some log


    // inform successor
    for(auto& successor: successors_)
    {
        std::unique_lock<std::mutex> lock(successor->mtx_); 
        successor->condition_ -= 1;
        if(successor->condition_ == 0)
        {
            // put it to the executor
            Executor::get_instance()->add_task([&successor]()
                {
                    successor->execute_();
                }
            );
        }
    }
    // count executed task
    {
        std::unique_lock<std::mutex> lock(schedule_->count_mtx_);
        schedule_->wait_executed -= 1;
        if(schedule_->wait_executed == 0) schedule_->count_cond_.notify_one();
    }
}

inline void Task::set_precede(Task* pre_task)
{
    precedes_.emplace_back(pre_task);
    pre_task->successors_.emplace_back(this);

    condition_ += 1;
}

void Task::set_precede(std::vector<Task*> pre_tasks)
{
    for(auto pre_task: pre_tasks)
    {
        set_precede(pre_task);
    }
}

void Task::set_precede(TaskPtr pre_task)
{
    set_precede(pre_task.get());
}
