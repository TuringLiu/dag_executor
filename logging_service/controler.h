// kafka连接器需要做一个定时器，定时的回收链接？还是做成单线程吧
// kafka is realtime task for grafana
// redis is non-realtime task for batch updation

#include <sw/redis++/redis++.h>
#include <iostream>
#include <map>
#include <chrono>
#include <unordered_map>
#include <prometheus/histogram.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/counter.h>
#include <atomic>
using namespace sw::redis;

class Controler
{
public:
    virtual void produce_(const std::string& metric_name, const std::map<std::string, std::string>& 
        label_name, const long long& counter, const long long& log_time) = 0;
    virtual void consume_() = 0;
};

class KafkaControler: Controler
{
public:
    KafkaControler();
    ~KafkaControler();

    virtual void produce_(const std::string& metric_name, const std::map<std::string, std::string>& 
        label_name, const long long& counter, const long long& log_time) override;
    virtual void consume_() override;
private:
};

class RedisControler: Controler
{
public:
    RedisControler();
    RedisControler(const std::string& key, const std::string& host, const std::string& port);

    virtual void produce_(const std::string& metric_name, const std::map<std::string, std::string>& 
        label_name, const long long& counter, const long long& log_time) override;
    virtual void consume_() override;
private:
    sw::redis::Redis redis_{"tcp://default:123456@127.0.0.1:6379/0"};
};

class PrometheusControler: Controler
{
public:
    // PrometheusControler();
    PrometheusControler(const std::string& host);
    // ~PrometheusControler();

    virtual void produce_(const std::string& metric_name, const std::map<std::string, std::string>& 
        label_name, const long long& counter, const long long& log_time) override;
    virtual void consume_() override;

private:
    std::shared_ptr<prometheus::Registry> registry_;
    prometheus::Exposer exposer_;
    std::atomic<bool> flag{false};
    // prometheus::Family<prometheus::Counter>& packet_counter;
};