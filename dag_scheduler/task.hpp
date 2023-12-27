#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include "executor.hpp"

/*
Info: Task own the function that would be executed. Besides Task get the DAG relatiionship rather than scheduler.
*/

class Scheduler;
class Task;
class Executor;

using TaskPtr = std::shared_ptr<Task>;

class Task
{
    friend class Scheduler;
public:
    template<typename CC>
    Task(CC&& cc);

    void execute_();

    void set_precede(Task*);
    void set_precede(std::vector<Task*>);

private:
    void pre_run_();
    void post_run_();

private:
    std::vector<Task*> precedes_;
    std::vector<Task*> successors_;

    Scheduler* schedule_;

    std::function<void()> work;
    std::string name_;
    int condition_;
    std::mutex mtx_;
};

template<typename CC>
Task::Task(CC&& cc): work{std::forward<CC>(cc)} {}

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
    // todo: inform successor and add some log

    // inform successor
    for(auto successor: successors_)
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
