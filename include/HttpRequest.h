#pragma once
#include"Buffer.h"
#include<iostream>
#include<string>
#include<algorithm>
#include<unordered_map>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class Buffer;

class HttpRequest
{
private:
    std::string method_;
    std::string path_;
    std::unordered_map<std::string,std::string> head_;
    // std::string body_;
    // std::unordered_map<std::string,std::string> body_;//消息体，默认全是json格式（key:value）
    json body_;                                             //消息体，暂时全是json格式
    bool parseThree(const std::string& _message,std::string& _Line,std::string& _Head,std::string& _Body);
                                                                        //传入消息及接收请求行、头、消息体的字符串
    bool parseLine(std::string& _Line);                                 //解析请求行
    bool parseHead(std::string& _Head);                                 //解析请求头
    bool parseBody(std::string& _body);                                 //解析消息体
public:
    HttpRequest();                                                  //构造函数
    ~HttpRequest();                                                 //析构函数
    bool parse(std::string& _buffer);                               //解析http消息
    std::string& GetMethod();                                       //得到方法
    std::string& GetPath();                                         //得到路径
    bool GetHeader(const std::string& _key,std::string& _val);      //得到请求体
    bool GetBody(const std::string& _key,std::string& _val);        //得到请求体
    json GetBody();                                                 //得到请求体

};