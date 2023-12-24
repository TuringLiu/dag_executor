#include <vector>

class Executor
{
public:
    template<typename CC>
    void add_task(CC&& cc);
private:
    
};