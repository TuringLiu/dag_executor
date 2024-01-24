// description: provide a interface to send log to kafka
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <thread>
#include "thread_pool/thread_pool.h"
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
const std::string kDefaultPsm = "NULL_PSM";

class LogSender
{
public:
    LogSender(const std::string& psm=kDefaultPsm, const unsigned int& sample_count = 1000);
    virtual void send(const std::string& metric_name, const LabelType& labels, const long long& counter) = 0;
protected:
    std::shared_ptr<Controler> controler_;

    const std::string psm_;
    // sample_rate = sample_count_ / sample_base_
    const unsigned int sample_count_;
    const unsigned int sample_base_{10};
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
    LocalSender(const std::string& local_path="./tmp.log");
    LocalSender(const std::string& local_path, std::shared_ptr<ThreadPool>& thread_pool);
    ~LocalSender();

    virtual void send(const std::string& metric_name, const LabelType& labels, const long long& counter) override;
private:
    std::string work_string_;
    std::string wait_string_;
    std::atomic<unsigned int> work_size_{0};
    const unsigned int log_cache_size_{10};
    std::mutex mutex_;

    const std::string local_path_;
    std::shared_ptr<ThreadPool> thread_pool_;
};

LocalSender::LocalSender(const std::string& local_path): LogSender(), local_path_{local_path}
{
    thread_pool_ = std::make_shared<ThreadPool>(1, 100);
}

LocalSender::LocalSender(const std::string& local_path, std::shared_ptr<ThreadPool>& thread_pool): LogSender("NULL_PSM", 1000), local_path_{local_path}, thread_pool_{thread_pool}
{
}
LocalSender::~LocalSender()
{
    if(work_size_ != 0)
    {
        thread_pool_->add_task([this](){
            save_string(this->work_string_, this->local_path_);
        });
    }
}

void LocalSender::send(const std::string& metric_name, const LabelType& labels, const long long& counter)
{
    // abosolute time from 1970-01-01 00:00:00 UTC, unit: nanosecond
    auto send_time = absolute_time_count();

    std::string log = serial_log(metric_name, labels, counter, send_time);

    std::unique_lock<std::mutex> lock(mutex_);
    work_string_ += log;
    work_size_++;

    if(work_size_ == log_cache_size_)
    {   
        log = work_string_;
        thread_pool_->add_task([log, this](){
            save_string(log, this->local_path_);
        });
        work_size_ = 0;
        work_string_ = "";
    }
}

void RedisSender::send(const std::string& metric_name, const LabelType& labels, const long long& counter)
{
    // abosolute time from 1970-01-01 00:00:00 UTC, unit: nanosecond
    auto send_time = absolute_time_count();

    std::string log = serial_log(metric_name, labels, counter, send_time);
}

