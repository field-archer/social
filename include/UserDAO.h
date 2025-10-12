#pragma once
#include"User.h"
#include"MYSQLConnectionPool.h"
using spMYSQLPOOL=std::shared_ptr<MYSQLConnectionPool>;

class UserDAO
{
private:
    spMYSQLPOOL mysqlPool_;     //共享的mysql连接池
public:
    UserDAO(spMYSQLPOOL _mysqlPool);            //构造函数
    ~UserDAO();                                 //析构函数
    void CreateUser(const User& _user);
};