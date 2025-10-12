#pragma once
#include<string>
#include<unordered_map>

class HttpResponse
{
private:
    int statusCode_;                                                //状态码
    std::string statusMessage_;                                     //状态信息
    std::unordered_map<std::string,std::string> head_;              //响应头（key-val）
    std::unordered_map<std::string,std::string> body_;              //响应体（key-val）
    // std::string body_;                                              //响应体

    // std::string ReturnHeader;                                       //最终完整响应头
public:
    HttpResponse();                                                 //构造函数
    ~HttpResponse();                                                //析构函数
    void SetStatusCode(int _code);                                  //设置状态吗
    void SetSatusMessage(const std::string& _message);              //设置状态消息
    void SetHead(const std::string& _key,const std::string& _val);  //设置响应头
    void SetBody(const std::string& _key,const std::string& _val);  //设置响应体

    int GetStatusCode_();                                           //得到状态码
    const std::string& GetstatusMessage();                          //得到状态信息
    bool GetHeader(const std::string& _key,std::string& _val);      //得到响应体
    bool GetBody(const std::string& _key,std::string& _val);        //得到响应体

    std::string GetAllHeader();                                     //返回head_所有数据
    std::string GetAllBody();                                       //返回body_所有数据

    void Set404();              //设置404 Not Found
    void Set200();              //设置200 OK

};