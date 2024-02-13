#pragma once
#include <chrono>
#include <fstream>
#include <cxxabi.h>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <vector>

using LabelType = std::unordered_map<std::string, std::string>;

// abosolute time from 1970-01-01 00:00:00 UTC, unit: nanosecond
inline long long absolute_time_count()
{
    return std::chrono::system_clock::now().time_since_epoch().count();
}

inline std::string serial_log(const std::string& metric_name, const LabelType& labels, const long long& counter, const long long& send_time)
{
    std::string log = metric_name + " " + std::to_string(send_time) + " " + std::to_string(counter) + " ";
    for(const auto& label: labels)
    {
        log += label.first + "=" + label.second + " ";
    }
    log += "\n";

    return log;
}

inline void save_string(const std::string& log, const std::string& path)
{  
    static std::mutex mutex_;
    std::unique_lock<std::mutex> lock(mutex_);
    std::ofstream outputFile(path, std::ios::out|std::ios::app);
    if (!outputFile.is_open()) {
        std::cout << "无法打开文件 " << path << std::endl;
        return ;
    }
    outputFile << log;
    outputFile.close();
}


// get chrono time from 1970-01-01 00:00:00 UTC
// time format: 2020-12-31-23-59-59
long long get_chrono_time(const std::string time)
{
    std::string tmp = "";
    std::vector<int> time_vec;
    for(auto& c: time)
    {
        if(c == '-') 
        {
            time_vec.push_back(std::stoi(tmp));
            tmp = "";
        }
        else tmp += c;
    }
    time_vec.push_back(std::stoi(tmp));
    if(time_vec.size() != 6) 
    {
        std::cout << "get_chrono_time error" << std::endl;
        return -1;
    }

    std::tm timeStruct{};
    timeStruct.tm_year = time_vec[0] - 1900;   // 年份需要减去 1900
    timeStruct.tm_mon = time_vec[1] - 1;          // 月份需要减去 1
    timeStruct.tm_mday = time_vec[2];
    timeStruct.tm_hour = time_vec[3];
    timeStruct.tm_min = time_vec[4];
    timeStruct.tm_sec = time_vec[5];
    std::time_t time_ = std::mktime(&timeStruct);
    std::chrono::system_clock::time_point specifiedTime = std::chrono::system_clock::from_time_t(time_);

    // 计算指定时间与 1970 年 1 月 1 日 00:00:00 之间的纳秒差值
    std::chrono::nanoseconds duration = specifiedTime - std::chrono::system_clock::from_time_t(0);
    // std::cout << "Duration since 1970: " << duration.count() << " ns" << std::endl;

    return duration.count();
}

template<typename T>
std::string get_type_name(const T&)
{
    int status;
    const std::type_info& type = typeid(T);
    char* demangledName = abi::__cxa_demangle(type.name(), nullptr, nullptr, &status);
    std::string type_name;
    if (status == 0) {
        type_name = demangledName;
        std::free(demangledName);
    } else {
        std::cerr << "Failed to demangle name" << std::endl;
        type_name = "failed to demangle name";
    }
    return type_name;
}

template<typename T>
std::string get_type_name()
{
    int status;
    const std::type_info& type = typeid(T);
    char* demangledName = abi::__cxa_demangle(type.name(), nullptr, nullptr, &status);
    std::string type_name;
    if (status == 0) {
        type_name = demangledName;
        std::free(demangledName);
    } else {
        std::cerr << "Failed to demangle name" << std::endl;
        type_name = "failed to demangle name";
    }
    return type_name;
}