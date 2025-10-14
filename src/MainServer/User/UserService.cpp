#include"UserService.h"

//构造函数
UserService::UserService(std::unique_ptr<UserDAO> _userDAO):userDAO_(std::move(_userDAO))
{

}
//构造函数
// UserService::UserService(const UserDAO& _userDAO):userDAO_(std::make_unique<UserDAO>(_userDAO))
// {

// }
//析构函数
UserService::~UserService()
{

}
//处理注册业务，返回userId(0表失败)
int UserService::HandleSignUp(const std::string& _name,const std::string& _email,const std::string& _passwd)
{
    //暂时没有要求，后续加（比如邮箱已有表以注册，邮箱应为有效邮箱，密码安全性要求等）

    try
    {
        User newUser(_name,_email,_passwd);
        return userDAO_->CreateUser(newUser);
    }
    catch(const std::exception& e)
    {//直接throw，无需补充
        throw std::runtime_error(e.what());
    }
}   
//处理登录业务，返回userId(0表失败)
int UserService::HandleLogIn(const std::string& _email,const std::string& _passwd)
{
    //暂时没有要求，后续加（比如邮箱未有表未注册，邮箱应为有效邮箱，密码长度不够等最低位数等）

    try
    {
        return userDAO_->LogIn(_email,_passwd);
    }
    catch(const std::exception& e)
    {//直接throw，无需补充
        throw std::runtime_error("登录出错"+std::string(e.what()));
    }
}