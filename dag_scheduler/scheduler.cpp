#include "task.h"
#include "executor.h"
#include "scheduler.h"


Scheduler::Scheduler(): name_{"default_scheduler"}
{
}

Scheduler::Scheduler(const std::string& name): name_{name}
{
}

void Scheduler::add_task(TaskPtr task)
{
    tasks_.emplace_back(task);
    task->schedule_ = this;
}

void Scheduler::execute_()
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
    // start task and end task to make some 
    wait_executed = tasks_.size();
    for(auto task: tasks_)
    {
        if(task->condition_ == 0) 
        {
            Executor::get_instance()->add_task([&task](){
                task->execute_();
            });
        }
    }
    // wait to executed

}

void Scheduler::post_run_()
{
    // todo: add log metrics

}