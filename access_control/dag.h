#include "access_control.h"
#include "dag_scheduler/scheduler.h"

class Context;
class Task;
using NodePtr = std::shared_ptr<Node>;

class DAG
{
public:
    DAG(std::string dag_name) : dag_name_(dag_name), scheduler_(dag_name) {}
    void add_node(NodePtr node);
    void run();
private:
    const std::string dag_name_;
    Scheduler scheduler_;
};

class Node
{
public:
    Node(std::string node_name) : node_name_(node_name) {}
    virtual void access_control(Context& ctx) = 0;
private:
    virtual void copy_from_ctx(Context& ctx) = 0;
    virtual void copy_on_success(Context& ctx) = 0;

    const std::string node_name_;
};

void DAG::add_node(NodePtr node)
{
    TaskPtr task = std::make_shared<Task>([node](){node->access_control();});
    scheduler_.add_task(task);
}
void DAG::run()
{
    scheduler_.execute();
}
