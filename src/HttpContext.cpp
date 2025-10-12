#include"HttpContext.h"
//空构造函数
// HttpContext::HttpContext():httpRequest_(),httpResponse_()//显示调用空构造函数
// {
// }
//构造函数
HttpContext::~HttpContext()
{

}   
//构造函数
HttpContext::HttpContext(HttpRequest& _httpRequest,HttpResponse& _httpResponse,HttpConnection& _httpConnection):
    httpRequest_(std::move(_httpRequest)),httpResponse_(std::move(_httpResponse)),httpConnection_(std::move(_httpConnection))
{
    //使用move避免开销
    //_httpRequest加const修饰会导致移动变拷贝
}
//从httpRequest_的消息体中从得到对应val_
bool HttpContext::GetRequestBody(const std::string& _key,std::string& _val)
{
    if(httpRequest_.GetBody(_key,_val)==false)
    {
        printf("获取%s时失败\n",_key.c_str());
        return false;
    }
    return true;
}
//发送404 NOT FOUND
void HttpContext::Send404()
{
    httpResponse_.Set404();
    httpConnection_.send(httpResponse_);
}
//发送200 OK
void HttpContext::Send200()
{
    httpResponse_.Set200();
    httpConnection_.send(httpResponse_);
}
//发送消息体为空的响应
void HttpContext::SendWithoutBOdy(int _code,const std::string& _message)
{
    httpResponse_.SetStatusCode(_code);
    httpResponse_.SetSatusMessage(_message);
    httpConnection_.send(httpResponse_);
}
//返回json（request的body_）
json HttpContext::GetRequestBody()
{
    return httpRequest_.GetBody();
}   