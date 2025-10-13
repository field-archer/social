#pragma once
#include"UserService.h"
#include"HttpContext.h"
#include<nlohmann/json.hpp>
using json=nlohmann::json;

class HttpContext;

// class DBConnection;
// class MYSQLConnectionPool;

// using spDBPool=std::shared_ptr<MYSQLConnectionPool>;
// using upDBSession=std::unique_ptr<mysqlx::Session>;
// using wpDBPool=std::weak_ptr<MYSQLConnectionPool>;

class UserController
{
private:
    std::unique_ptr<UserService> userService_;
public: 
    UserController(const std::unique_ptr<UserService>& _userService)=delete;            //禁用拷贝构造
    UserController& operator=(const std::unique_ptr<UserService>& _userService)=delete; //禁用拷贝赋值
    UserController(std::unique_ptr<UserService>&& _userService) noexcept;               //移动构造函数
    UserController& operator=(std::unique_ptr<UserService>&& _userService)noexcept;     //移动赋值
    ~UserController()=default;                                                          //析构函数

    bool HandleSignUp(std::unique_ptr<HttpContext> _context);                           //注册函数
    bool HandleLogIn(std::unique_ptr<HttpContext> _context);                            //登录函数
};