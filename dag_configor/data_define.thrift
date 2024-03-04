namespace recommend

struct UserData
{
    1: required string name,
    2: required i32 age,
    3: required string sex,
    4: required string location,
    5: i32 device_id,
}

struct ItemData
{
    1: required string item_id,
    2: required string location,
    3: required map<string, list<double>> scores,
    4: required list<string> tags
}

struct ThreadConfig
{
    1: required i32 num,
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
    3: required map<string, list<double>> user_scores,
    4: required map<i64, ItemData> item_data,
}

struct Ecommerce
{
    1: required list<i64> list,
    2: required list<string> model_list,
    3: required map<string, list<double>> user_scores,
    4: required map<i64, ItemData> item_data,
}

struct Live
{
    1: required list<i64> list,
    2: required list<string> model_list,
    3: required map<string, list<double>> user_scores,
    4: required map<i64, ItemData> item_data,
}

# 仅可读，是否应该删去？这应该是在业务框架之外的
struct Engineering
{
    1: required ThreadConfig thread_conf,
    2: required CircuitBreaker breaker_conf
}

# common中的数据仅可读，不可写【即只能出现0和1】
struct Common
{
    1: UserData user_data
}

struct Request
{
    1: required Engineering eng,
    2: required ShortVideo video,
    3: required Ecommerce ecom,
    4: required Live live,
    5: required Common comm,
}