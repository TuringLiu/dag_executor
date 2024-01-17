// description: provide a interface to send log to kafka
#include <string>
#include <memory>
#include <unordered_map>
#include "controler.h"
#include "util/basic_function.h"

// design rule: don't design the inherit relationship over two layer

// enum SendType
// {
//     SEND_KAFkA,
//     SEND_REDIS,
//     SEND_PROMETHEUS,
//     SEND_LOCAL
// };

using LabelType = std::unordered_map<std::string, std::string>;

class LogSender
{
public:
    LogSender() = delete;
    LogSender(const std::string& psm, const unsigned int& sample_count = 1000);
    virtual void send(const std::string& metric_name, const LabelType& labels, const long long& counter) = 0;
private:
    std::shared_ptr<Controler> controler_;

    const std::string psm_;
    // sample_rate = sample_count_ / sample_base_
    const unsigned int sample_count_;
    const unsigned int sample_base_{1000};
};
LogSender::LogSender(const std::string& psm, const unsigned int& sample_count): psm_{psm}, sample_count_{sample_count}
{
}

class KafkaSender : LogSender
{
public:
    virtual void send(const std::string& metric_name, const LabelType& labels, const long long& counter) override;
};

class RedisSender : LogSender
{
public:
    virtual void send(const std::string& metric_name, const LabelType& labels, const long long& counter) override;
};

class PrometheusSender : LogSender
{
public:
    virtual void send(const std::string& metric_name, const LabelType& labels, const long long& counter) override;
};

class LocalSender : LogSender
{
public:
    LocalSender(const std::string& local_path);

    virtual void send(const std::string& metric_name, const LabelType& labels, const long long& counter) override;
private:
    const std::string local_path_;
};

LocalSender::LocalSender(const std::string& local_path="/tmp.log"): local_path_{local_path}, LogSender("NULL_PSM", 1000)
{
}


void LocalSender::send(const std::string& metric_name, const LabelType& labels, const long long& counter)
{
    // abosolute time from 1970-01-01 00:00:00 UTC, unit: nanosecond
    auto send_time = absolute_time_count();

    std::string log = metric_name + " " + std::to_string(send_time) + " " + std::to_string(counter) + " ";
    for(const auto& label: labels)
    {
        log += label.first + "=" + label.second + " ";
    }
    log += "\n";

    std::cout << "LocalSender: " << log << std::endl;
}