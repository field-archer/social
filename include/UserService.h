#pragma once
#include"UserDAO.h"

class UserDAO;

//业务层

class UserService
{
private:
    std::unique_ptr<UserDAO> userDAO_;
public:
    UserService(std::unique_ptr<UserDAO> _userDAO);              //构造函数
    // UserService(const UserDAO& _userDAO);              //构造函数
    ~UserService();             //析构函数

    bool HandleSignUp(const std::string& name,const std::string& email,const std::string& passwd);          //处理注册业务
    bool HandleLogIn(const std::string& email,const std::string& passwd);                                 //处理登录业务
};