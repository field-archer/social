#pragma once
#include"User.h"
#include"MYSQLConnectionPool.h"


class MYSQLConnectionPool;
class DBConnection;

using spDBPool=std::shared_ptr<MYSQLConnectionPool>;


class UserDAO
{
private:
    spDBPool mysqlPool_;     //共享的mysql连接池
public:
    UserDAO(spDBPool _mysqlPool);            //构造函数
    ~UserDAO();                                 //析构函数
    void CreateUser(const User& _user);                                 //创建用户
    bool LogIn(const std::string& _email,const std::string& _passwd);   //登录
};