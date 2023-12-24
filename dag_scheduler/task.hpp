#include <vector>
#include <memory>
#include <functional>


/*
Info: Task own the function that would be executed. Besides Task get the DAG relatiionship rather than scheduler.
*/

class Task
{
public:
    template<typename CC>
    Task(CC&& cc);

    void execute_();

    void set_precede(Task*);
    void set_precede(std::vector<Task*>);

private:
    std::vector<Task*> precedes_;
    std::vector<Task*> successors_;

    std::function<void()> work;
};

template<typename CC>
Task::Task(CC&& cc): work{std::forward<CC>(cc)} {}

void Task::execute_()
{
    work();
}

void Task::set_precede(Task* pre_task)
{
    precedes_.emplace_back(pre_task);
    pre_task->successors_.emplace_back(this);
}

void Task::set_precede(std::vector<Task*> pre_tasks)
{
    precedes_.insert(precedes_.end(), pre_tasks.begin(), pre_tasks.end());
    for(auto pre_task: pre_tasks)
    {
        pre_task->successors_.emplace_back(this);
    }
}
