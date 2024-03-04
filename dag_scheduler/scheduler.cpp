#include "task.h"
#include "executor.h"
#include "scheduler.h"
#include <iostream>

Scheduler::Scheduler(): name_{"default_scheduler"}
{
}

Scheduler::Scheduler(const std::string& name): name_{name}
{
}

Scheduler::~Scheduler()
{
    Executor::works_over();
}

void Scheduler::add_task(TaskPtr task)
{
    tasks_.emplace_back(task);
    task->schedule_ = this;
}



void Scheduler::execute()
{
    pre_run_();
    work_();
    post_run_();
}


void Scheduler::pre_run_()
{
    //  todo: init log metrics
}

void Scheduler::work_()
{
    // only start task
    wait_executed = tasks_.size();
    for(auto& task: tasks_)
    {
        if(task->condition_ == 0) 
        {
            Executor::get_instance()->add_task([&task](){
                task->execute_();
            });
        }
    }
    // end scheduler after all tasks done
    {
        std::unique_lock<std::mutex> lock(count_mtx_);
        count_cond_.wait(lock, [this](){
            return this->wait_executed == 0;
        });
    }

}

void Scheduler::post_run_()
{
    // todo: add log metrics
    std::cout << "scheduler post_run" << std::endl;
}