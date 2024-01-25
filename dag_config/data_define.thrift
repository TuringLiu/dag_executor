namespace recommend

struct UserData
{
    1: required string name,
    2: required i32 age,
    3: required string sex,
    4: required string location,
    5: required i32 device_id,
}

struct ItemData
{
    1: required string item_id,
    1: required map<string, list<double>> item_scores,
    3: required list<string> tags
}

struct Log
{
    1: required string latency,
    2: required string video_num
}

struct ThreadConfig
{
    1：required i32 num,
    2: required i32 task_num
}

struct CircuitBreaker
{
    1: required map<string, i32> max_latency,
    2: required map<string, string> replace_node
}

struct ShortVideo
{
    1: required list<i64> list
    2: required list<string> model_list
}

struct Ecommerce
{
    1: required list<i64> list
    2: required list<string> model_list
}

struct Live
{
    1: required list<i64> list
    2: required list<string> model_list
}

struct Engineering
{
    1: required Log log,
    2: required ThreadConfig thread_conf,
    3: required CircuitBreaker breaker_conf
}


struct Common
{
    1: UserData user_data,
    2: ItemData item_data,
}

struct Request
{
    1: required Engineering eng,
    2: required ShortVideo video,
    3: required Ecommerce ecom,
    4: required Live live,
    5: required Common comm,
}