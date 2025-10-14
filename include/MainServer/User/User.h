#pragma once
#include<string>
#include<iostream>
#include<nlohmann/json.hpp>
using json=nlohmann::json;

class User
{
private:
    std::string name_;
    std::string email_;
    std::string passwd_;
    int id_;
public:
    User(const std::string& _name,const std::string& _email,const std::string& _passwd);        //默认构造函数
    User(const json& j);                //构造函数
    ~User();                            //析构函数

    std::string GetName() const;              //得到姓名
    std::string GetEmail() const;             //得到邮箱
    std::string GetPasswd() const;            //得到密码

    void SetId(int _id);                    //设置id
    
    
};