#pragma once
#include<string>
#include<unordered_map>

class HttpResponse
{
private:
    int statusCode_;
    std::string statusMessage_;
    std::unordered_map<std::string,std::string> Head_;
    std::string body_;

    std::string ReturnHeader;
public:
    HttpResponse();
    ~HttpResponse();
    void SetStatusCode(int _code);
    void SetSatusMessage(const std::string& _message);
    void SetHead(const std::string& _key,const std::string& _val);
    void SetBody(const std::string& _message);

    int GetStatusCode_();
    const std::string& GetstatusMessage();
    const std::string& GetHeader();
    const std::string& GetBody();
};