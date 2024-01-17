#include <chrono>

// abosolute time from 1970-01-01 00:00:00 UTC, unit: nanosecond
inline long long absolute_time_count()
{
    return std::chrono::system_clock::now().time_since_epoch().count();
}