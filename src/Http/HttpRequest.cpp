#include"HttpRequest.h"
HttpRequest::HttpRequest():method_(""),path_("")
{
    body_.clear();
    head_.clear();
}
HttpRequest::~HttpRequest()
{

}
//得到方法
std::string& HttpRequest::GetMethod()
{
    return method_;
}
//得到路径
std::string& HttpRequest::GetPath()
{
    return path_;
}
//得到响应头
bool HttpRequest::GetHeader(const std::string& _key,std::string& _val)
{
    
    if(head_.find(_key)==head_.end())return false;
    _val=head_[_key];
    return true;
}
//返回请求体字符串
std::string HttpRequest::GetBody()
{
    return body_;
}

//传入消息及接收请求行、头、消息体的字符串
//解析出的行，头均以一个\r\n结尾
bool HttpRequest::parseThree(const std::string& _message,std::string& _Line,std::string& _Head,std::string& _Body)
{
    size_t ind=0;
    //解析请求行
    ind=_message.find("\r\n",ind);
    if(ind==std::string::npos)return false;
    ind+=2;
    _Line=_message.substr(0,ind);
    //解析请求头
    size_t headStart=ind;
    ind=_message.find("\r\n\r\n",headStart);
    if(ind==std::string::npos)return false;
    ind+=4;
    _Head=_message.substr(headStart,ind-2-headStart);
    //解析body
    size_t bodyStart=ind;
    if(bodyStart>=_message.size())_Body.clear();
    else 
    {
        _Body=_message.substr(bodyStart);
        _Body+="\r\n";
    }
    return true;
}
//解析请求行
bool HttpRequest::parseLine(std::string& _Line)
{
    size_t ind=0;
    //解析方法
    size_t methodStart=ind;
    ind=_Line.find(" ",methodStart);
    if(ind==std::string::npos)return false;
    method_=_Line.substr(methodStart,ind-methodStart);
    //解析路径
    size_t pathStart=ind+1;
    ind=_Line.find(" ",pathStart);
    if(ind==std::string::npos)return false;
    path_=_Line.substr(pathStart,ind-pathStart);
    //解析HTTP版本
    size_t versionStart=ind+1;
    ind=_Line.find("\r\n",versionStart);
    if(ind==std::string::npos)return false;
    std::string httpVersion;
    httpVersion=_Line.substr(versionStart,ind-versionStart);
    return true;
}

//解析请求头
//​​parseHead函数没有处理重复头部字段
//格式严格要求，传入头部必须以一个\r\n结尾
bool HttpRequest::parseHead(std::string& _Head)
{
    size_t ind=0;
    while(true)
    {
        //sta----ind表一行
        size_t sta=ind;
        ind=_Head.find("\r\n",ind);
        if(ind==std::string::npos)break;
        //分割出key和val
        size_t keyEnd=_Head.find(":",sta);
        if(keyEnd==std::string::npos)return false;//未找到key和val
        //跳过空格
        size_t valStart=keyEnd+1;
        while(valStart<ind&&_Head[valStart]==' ')valStart++;
        if(valStart>=ind)return false;              //没有val
        //提取key和val
        std::string key=_Head.substr(sta,keyEnd-sta);
        std::string val=_Head.substr(valStart,ind-valStart);
        std::transform(key.begin(),key.end(),key.begin(),::tolower);
        // std::transform(val.begin(),val.end(),val.begin(),::tolower);
        //存入map
        head_[key]=val;
        //迭代ind
        ind+=2;
    }
    return true;
}

//解析消息体（暂时默认全是json格式）
bool HttpRequest::parseBody(std::string& _body)
{    
    body_=_body;
    return true;
}
bool HttpRequest::parse(std::string& _message)
{
    //解析出请求行，请求头，消息体
    std::string Line_,head_,body_;
    if(parseThree(_message,Line_,head_,body_)==false)return false;

    //解析请求行
    if(parseLine(Line_)==false)return false;
    //解析请求头
    if(parseHead(head_)==false)return false;
    //解析消息体
    if(parseBody(body_)==false)return false;
    return true;
}


