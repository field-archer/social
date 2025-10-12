#include"HttpResponse.h"
//构造函数
HttpResponse::HttpResponse():
                statusCode_(0),statusMessage_("")
{
    head_.clear();
    body_.clear();
}
//析构函数
HttpResponse::~HttpResponse()
{
    
}
//得到状态码
int HttpResponse::GetStatusCode_()
{
    return statusCode_;
}
//得到状态信息
const std::string& HttpResponse::GetstatusMessage()
{
    return statusMessage_;
}
//得到响应头
bool HttpResponse::GetHeader(const std::string& _key,std::string& _val)
{
    if(head_.find(_key)==head_.end())return false;
    _val=head_[_key];
    return true;
}
//得到响应体
bool HttpResponse::GetBody(const std::string& _key,std::string& _val)
{
    if(body_.find(_key)==body_.end())return false;//未找到对应key
    _val=body_[_key];
    return true;
}
//设置状态码 
void HttpResponse::SetStatusCode(int _code)
{
    statusCode_=_code;
}
//设置状态信息
void HttpResponse::SetSatusMessage(const std::string& _message)
{
    statusMessage_=_message;
}
//设置响应头
void HttpResponse::SetHead(const std::string& _key,const std::string& _val)
{
    head_[_key]=_val;
}
//设置响应体
void HttpResponse::SetBody(const std::string& _key,const std::string& _val)
{
    body_[_key]=_val;
}
//返回head_所有数据
std::string HttpResponse::GetAllHeader()
{
    std::string returnHeader="";
    for(auto pair:head_)
    {
        returnHeader+=pair.first;
        returnHeader+=": ";
        returnHeader+=pair.second;
    }
    return returnHeader;
}
//返回body_所有数据
std::string HttpResponse::GetAllBody()
{
    std::string returnBody="";
    for(auto pair:head_)
    {
        returnBody+=pair.first;
        returnBody+=": ";
        returnBody+=pair.second;
    }
    return returnBody;
}
//设置404 Not Found
void HttpResponse::Set404()
{
    SetStatusCode(404);
    SetSatusMessage("NOT Found");
}
//设置200 OK
void HttpResponse::Set200()
{
    SetStatusCode(200);
    SetSatusMessage("OK");
}