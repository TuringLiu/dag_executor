#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>


// singleton 
class Executor
{
public:
    void add_task(std::function<void()> f);
    static Executor* get_instance();

    Executor(const Executor&) = delete;
    Executor& operator = (const Executor) = delete;

private:
    Executor(){};
    ~Executor(){};

    static Executor* instance_;
};
