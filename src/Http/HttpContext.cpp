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
//返回请求体,慎用，不要存储返回值
HttpRequest& HttpContext::GetRequest()
{
    return httpRequest_;
}
//返回响应体，慎用，不要存储返回值
HttpResponse& HttpContext::GetResponse()
{
    return httpResponse_;
}
//返回连接，慎用，不要存储返回值
HttpConnection& HttpContext::GetConnection()
{
    return httpConnection_;
}
//设置常见响应头
void HttpContext::SetResponseUsefulHead()
{
    httpResponse_.SetHead("Content-Type", "application/json");
    httpResponse_.SetHead("Content-Length", std::to_string(httpResponse_.GetBody().size()));
}

//设置http响应状态码
void HttpContext::SetResponseStatusCode(int _code)
{
    httpResponse_.SetStatusCode(_code);
}
//设置http响应状态信息
void HttpContext::SetResponseStatusMessage(const std::string& _message)
{
    httpResponse_.SetSatusMessage(_message);
}
//增加http响应头部
void HttpContext::AddResponseHead(const std::string _key,const std::string& _val)
{
    httpResponse_.SetHead(_key,_val);
}
//设置响应体状态码和状态行和常用请求头
void HttpContext::SetReSponseStatusANDUsefulHead(int _code,const std::string& _message)
{
    SetResponseStatusCode(_code);
    SetResponseStatusMessage(_message);
    SetResponseUsefulHead();
}
//设置http响应响应头
void HttpContext::SetResponseBody(std::string&& _body)
{
    httpResponse_.SetBody(std::move(_body));
}
//发送http响应
void HttpContext::Send()
{
    httpConnection_.send(httpResponse_);
}