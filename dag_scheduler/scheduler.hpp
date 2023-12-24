#include <vector>
#include <memory>
#include <string>
#include "task.hpp"

class Scheduler
{
public:
    Scheduler();
    
    template<typename CC>
    void add_task(CC&& cc);
    void run_();
private:
    std::vector<TaskPtr> tasks_;
};
