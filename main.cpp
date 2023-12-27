#include "dag_scheduler/scheduler.h"
#include "dag_scheduler/task.h"

#include <iostream>
#include <thread>

int main()
{
    std::cout << "this is a test!" << std::endl;
    TaskPtr task = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 1" << std::endl;});
    TaskPtr task_2 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 2" << std::endl;});
    TaskPtr task_3 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 3" << std::endl;});
    TaskPtr task_4 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 4" << std::endl;});

    Scheduler dag;
    dag.add_task(task);
    dag.add_task(task_2);
    dag.add_task(task_3);
    dag.add_task(task_4);

    task->set_precede(task_2);
    task_2->set_precede(task_3);
    task_3->set_precede(task_4);

    dag.execute_();

    return 0;
}