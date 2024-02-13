#include "access_control.h"

class Context;
class Scheduler;
class Task;

class DAG
{
public:
    DAG(std::string dag_name) : dag_name_(dag_name) {}
    void add_node();
private:
    const std::string dag_name_;
    Scheduler scheduler_;
};

class Node
{
public:
    Node(std::string node_name) : node_name_(node_name) {}
    virtual void access_control(Context ctx) = 0;
private:
    const std::string node_name_;
}
