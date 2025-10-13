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
void HttpContext::SetUsefulHead()
{
    httpResponse_.SetHead("Content-Type", "application/json");
    httpResponse_.SetHead("Content-Length", std::to_string(httpResponse_.GetBody().size()));
}