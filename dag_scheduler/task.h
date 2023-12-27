#include <vector>
#include <memory>
#include <functional>
#include <mutex>

/*
Info: Task own the function that would be executed. Besides Task get the DAG relatiionship rather than scheduler.
*/

class Scheduler;
class Task;

using TaskPtr = std::shared_ptr<Task>;

class Task
{
    friend class Scheduler;
public:
    template<typename CC>
    Task(CC&& cc);

    void set_precede(Task*);
    void set_precede(std::vector<Task*>);
    void set_precede(TaskPtr);

private:
    void execute_();
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