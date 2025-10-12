#pragma once
#include"UserDAO.h"

//业务层

class UserService
{
private:
    std::unique_ptr<UserDAO> userDAO_;
public:
    UserService();              //构造函数
    UserService(const UserDAO& _userDAO);              //构造函数
    ~UserService();             //析构函数

    void HandleSignUp(const std::string& name,const std::string& email,const std::string& passwd);          //处理注册业务
    void HandleLoginIn(const std::string& email,const std::string& passwd);                                 //处理登录业务
};