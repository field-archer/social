#pragma once
#include"Buffer.h"
#include<string>
#include<algorithm>
#include<unordered_map>
class Buffer;

class HttpRequest
{
private:
    std::string method_;
    std::string path_;
    std::unordered_map<std::string,std::string> head_;
    std::string body_;
    bool parseThree(const std::string& _message,std::string& _Line,std::string& _Head,std::string& _Body);
                                                                        //传入消息及接收请求行、头、消息体的字符串
    bool parseLine(std::string& _Line);                                 //解析请求行
    bool parseHead(std::string& _Head);                                 //解析请求头
    bool parseBody(std::string& _body);                                 //解析消息体
public:
    HttpRequest();
    ~HttpRequest();
    bool parse(std::string& _buffer);
    std::string& GetMethod();
    std::string& GetBody();
    std::string& GetPath();

};