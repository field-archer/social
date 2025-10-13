#pragma once
#include<time.h>
#include<string.h>
#include<string>
class TimeStamp
{
private:
    time_t time_;
public:
    TimeStamp();
    TimeStamp(time_t _time);
    ~TimeStamp();
    static TimeStamp now();
    time_t ToInt() const;
    std::string ToString() const;
};