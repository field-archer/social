#include"HttpResponse.h"
HttpResponse::HttpResponse():
                statusCode_(0)
{
    
}
HttpResponse::~HttpResponse()
{
    
}
int HttpResponse::GetStatusCode_()
{
    return statusCode_;
}
const std::string& HttpResponse::GetstatusMessage()
{
    return statusMessage_;
}
const std::string& HttpResponse::GetHeader()
{
    ReturnHeader="";
    for(auto pair:Head_)
    {
        ReturnHeader+=pair.first;
        ReturnHeader+=": ";
        ReturnHeader+=pair.second;
        ReturnHeader+="\r\n";
    }
    return ReturnHeader;
}
const std::string& HttpResponse::GetBody()
{
    return body_;
}
void HttpResponse::SetStatusCode(int _code)
{
    statusCode_=_code;
}
void HttpResponse::SetSatusMessage(const std::string& _message)
{
    statusMessage_=_message;
}
void HttpResponse::SetHead(const std::string& _key,const std::string& _val)
{
    Head_[_key]=_val;
}
void HttpResponse::SetBody(const std::string& _message)
{
    body_=_message;
}