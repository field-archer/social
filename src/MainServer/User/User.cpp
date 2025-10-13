#include"User.h"

//构造函数
User::User(const json& j):
    name_(j["name"].get<std::string>()),email_(j["email"].get<std::string>()),passwd_(j["passwd"].get<std::string>())
{

}   
//默认构造函数
User::User(const std::string& _name,const std::string& _email,const std::string& _passwd):
    name_(_name),email_(_email),passwd_(_passwd)
{
    
}
//析构函数
User::~User()
{

}
//得到姓名
std::string User::GetName() const
{
    return name_;
}
//得到邮箱

std::string User::GetEmail() const
{
    return email_;
}
//得到密码
std::string User::GetPasswd() const
{
    return passwd_;
}
