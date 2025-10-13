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
    // bool GetRequestBody(const std::string& _key,std::string& _val);         //从httpRequest_的消息体中从得到对应val_
    // json GetRequestBody();                                                  //返回json（request的body_）
    // std::string GetRequestBody();                                           //返回消息体字符串
    HttpRequest& GetRequest();                                                  //返回请求体，慎用，不要存储返回值
    HttpResponse& GetResponse();                                                //返回响应体，慎用，不要存储返回值
    HttpConnection& GetConnection();                                            //返回连接，慎用，不要存储返回值

    void SetUsefulHead();                                                       //设置常见响应头

    // void SendWithoutBOdy(int _code,const std::string& _message);            //发送消息体为空的响应
    // void Send404();             //发送404 NOT FOUND
    // void Send200();             //发送200 OK
};