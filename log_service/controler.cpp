#include "controler.h"

RedisControler::RedisControler()
{
    RedisControler("123456", "127.0.0.1", "6379");
}

RedisControler::RedisControler(const std::string& key, const std::string& host, const std::string& port)
{
    const std::string target = "tcp://default:" + key + "@" + host + ":" + port + "/0";
    redis_ = sw::redis::Redis(target);
}

void RedisControler::produce_(const std::string& metric_name, const std::map<std::string, std::string>& 
        label_name, const long long& counter, const long long& log_time)
{
    std::cout << redis_.ping() << std::endl;

    redis_.set("key", "world");
    auto value = redis_.get("key");
    if (value) {
        // Dereference val to get the returned value
        std::cout << *value << std::endl;
    }
}

void RedisControler::consume_()
{
    
}

// PrometheusControler::PrometheusControler()
// {
//     PrometheusControler("127.0.0.1:8080");
// }

PrometheusControler::PrometheusControler(const std::string& host): exposer_{host}
{
    registry_ = std::make_shared<prometheus::Registry>();
}

// to prometheus
void PrometheusControler::produce_(const std::string& metric_name, const std::map<std::string, std::string>& 
        label_name, const long long& counter, const long long& log_time)
{
	// auto& histogram_family = prometheus::BuildHistogram()
	// 	.Name(metric_name)
	// 	.Help("no description")
	// 	.Register(*registry_);
    flag = true;
    
	// // add and remember dimensional data, incrementing those is very cheap
	// auto& histogram = histogram_family.Add(label_name, prometheus::Histogram::BucketBoundaries{0, 1, 2});
    // histogram.Observe(counter);
    // histogram.Collect();

    auto& packet_counter = prometheus::BuildCounter()
                             .Name(metric_name)
                             .Help("Number of observed packets")
                             .Register(*registry_);
    // ask the exposer to scrape the registry on incoming HTTP requests
    if(flag == false) exposer_.RegisterCollectable(registry_);

    // add and remember dimensional data, incrementing those is very cheap
    auto& tcp_rx_counter = packet_counter.Add({{"method", "GET"}});
    tcp_rx_counter.Increment(counter);
	
	// ask the exposer to scrape the registry on incoming HTTP requests
    if(flag == false) exposer_.RegisterCollectable(registry_);
}

// from where?
void PrometheusControler::consume_()
{
    
}
