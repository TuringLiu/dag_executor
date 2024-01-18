#include <string>

class LogConsumer
{
public:
    virtual void consume(const std::string& log) = 0;
};





