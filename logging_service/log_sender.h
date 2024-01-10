// description: provide a interface to send log to kafka
#include <string>
#include <memory>
#include "controler.h"

class LogSender
{
public:
    LogSender();
    ~LogSender();

    // async send interface
    void send(const std::string& metric_name, const double& time);
    void send(const std::string& metric_name, const unsigned int& time);
private:
    std::shared_ptr<Controler> controler_;
    const std::string psm_;

    // sample_rate = sample_count_ / sample_base_
    const unsigned int sample_count_;
    const unsigned int sample_base_{1000};
};