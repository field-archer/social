#pragma once
#include"MYSQLConnectionPool.h"
#include<mysqlx/xdevapi.h>
// class 

class MYSQLConnectionPool;
using spDBPool=std::shared_ptr<MYSQLConnectionPool>;
using upDBSession=std::unique_ptr<mysqlx::Session>;
using wpDBPool=std::weak_ptr<MYSQLConnectionPool>;



class DBConnection
{
private:
    wpDBPool DBPool_;                   //mysql连接池
    upDBSession dbConnection_;          //mysql连接
public:
    DBConnection(spDBPool _DBPool);     //构造函数
    DBConnection(spDBPool _DBPool,upDBSession _DBConnection);     //构造函数

    DBConnection(const DBConnection& _DBConnection)=delete;             //删除拷贝构造函数
    DBConnection& operator=(const DBConnection& _DBConnection)=delete;  //删除拷贝赋值
    DBConnection(DBConnection&& _DBConnection) noexcept;                //移动构造函数
    DBConnection& operator=(DBConnection&& _DBConnection) noexcept;     //重载移动赋值

    ~DBConnection();                    //析构函数

    upDBSession GetSession();           //返回原始连接
    mysqlx::Session& operator*() const; //重载*运算符，返回Session引用

    bool isValid();                     //连接是否可用

};