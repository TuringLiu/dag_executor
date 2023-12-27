#include "dag_scheduler/scheduler.hpp"
#include "dag_scheduler/task.hpp"
#include <iostream>
#include <thread>

int main()
{
    std::cout << "this is a test!" << std::endl;
    Task task([](){ std::cout << "this is a test for lamda function!" << std::endl;});
    task.execute_();



    return 0;
}