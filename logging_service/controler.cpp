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

void RedisControler::produce_()
{

}

void RedisControler::consume_()
{
    
}


int main(void) {
    auto redis = Redis("tcp://default:123456@127.0.0.1:6379/0");
    std::cout << redis.ping() << std::endl;

    redis.set("key", "world");
    auto value = redis.get("key");
    if (value) {
        // Dereference val to get the returned value
        std::cout << *value << std::endl;
    }
}