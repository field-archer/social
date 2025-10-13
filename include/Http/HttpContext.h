#pragma once
#include"HttpRequest.h"
#include"HttpResponse.h"
#include"HttpConnection.h"

class HttpContext
{
private:
    HttpRequest httpRequest_;
    HttpResponse httpResponse_;
    HttpConnection httpConnection_;
public:
    // HttpContext();                                                          //空构造函数
    HttpContext(HttpRequest& _httpRequest,HttpResponse& _httpResponse,HttpConnection& _httpConnection);     //构造函数
    ~HttpContext();                                                         //构造函数
    HttpRequest& GetRequest();                                                  //返回请求体，慎用，不要存储返回值
    HttpResponse& GetResponse();                                                //返回响应体，慎用，不要存储返回值
    HttpConnection& GetConnection();                                            //返回连接，慎用，不要存储返回值

    void SetUsefulHead();                                                       //设置常见响应头

};