#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>
#include <iostream>

// scheduler负责启动初始结点和控制结束结点，属于一张DAG图的抽象

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
    ~Scheduler();

    void add_task(TaskPtr);
    void execute();

private:
    void pre_run_();
    void work_();
    void post_run_();

private:
    std::vector<TaskPtr> tasks_;
    std::string name_;

    int wait_executed;
    std::mutex count_mtx_;
    std::condition_variable count_cond_;
};