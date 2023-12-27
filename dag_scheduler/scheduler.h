#include <vector>
#include <memory>
#include <string>
#include <mutex>

class Task;
using TaskPtr = std::shared_ptr<Task>;
class Scheduler;
using SchedulerPtr = std::shared_ptr<Scheduler>;

class Scheduler
{
    friend class Task;
public:
    Scheduler();
    Scheduler(const std::string& name);

    void add_task(TaskPtr);
    void execute_();

private:
    void pre_run_();
    void work_();
    void post_run_();

private:
    std::vector<TaskPtr> tasks_;
    std::string name_;

    int wait_executed;
    std::mutex count_mtx_;
};