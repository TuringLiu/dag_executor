// kafka连接器需要做一个定时器，定时的回收链接？还是做成单线程吧
// kafka is realtime task for grafana
// redis is non-realtime task for batch updation

#include <sw/redis++/redis++.h>
#include <iostream>
using namespace sw::redis;

class Controler
{
public:
    virtual void produce_() = 0;
    virtual void consume_() = 0;
};

class KafkaControler: Controler
{
public:
    KafkaControler();
    ~KafkaControler();

    virtual void produce_() override;
    virtual void consume_() override;
private:
};

class RedisControler: Controler
{
public:
    RedisControler();
    RedisControler(const std::string& key, const std::string& host, const std::string& port);

    virtual void produce_() override;
    virtual void consume_() override;
private:
    sw::redis::Redis redis_{"tcp://default:123456@127.0.0.1:6379/0"};
};