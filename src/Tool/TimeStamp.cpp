#include"TimeStamp.h"
TimeStamp::TimeStamp():time_(time(0))
{

}
TimeStamp::TimeStamp(time_t _time):time_(_time)
{

}
TimeStamp::~TimeStamp()
{

}
time_t TimeStamp::ToInt() const
{
    return time_;
}
std::string TimeStamp::ToString() const
{
    char buf[128];
    memset(buf,0,sizeof(buf));
    tm *tm_time=localtime(&time_);
    sprintf(buf,"%04d-%02d-%02d-%02d-%02d-%02d",tm_time->tm_year,tm_time->tm_mon,tm_time->tm_mday,
                                                tm_time->tm_hour,tm_time->tm_min,tm_time->tm_sec);
    return buf;
}
TimeStamp TimeStamp::now()
{
    return TimeStamp();
}