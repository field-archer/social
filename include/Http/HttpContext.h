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

    void SetResponseStatusCode(int _code);                                      //设置http响应状态码
    void SetResponseStatusMessage(const std::string& _message);                 //设置http响应状态信息  
    void SetResponseUsefulHead();                                               //设置http响应常见头
public:
    // HttpContext();                                                          //空构造函数
    HttpContext(HttpRequest& _httpRequest,HttpResponse& _httpResponse,HttpConnection& _httpConnection);     //构造函数
    ~HttpContext();                                                         //构造函数
    HttpRequest& GetRequest();                                                  //返回请求体，慎用，不要存储返回值
    HttpResponse& GetResponse();                                                //返回响应体，慎用，不要存储返回值
    HttpConnection& GetConnection();                                            //返回连接，慎用，不要存储返回值

    // std::string GetRequestBody();                                               //返回请求体

    void SetReSponseStatusANDUsefulHead(int _code,const std::string& _message); //设置响应体状态码和状态行和常用请求头
    void SetResponseBody(std::string&& _body);                                  //设置http响应响应头
    void AddResponseHead(const std::string _key,const std::string& _val);       //增加http响应头部

    void Send();                                                                //发送http响应



};