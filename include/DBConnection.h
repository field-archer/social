#pragma once
#include"MYSQLConnectionPool.h"
#include<mysqlx/xdevapi.h>



class DBConnection
{
private:
    spDBPool DBPool_;                   //mysql连接池
    upDBSession DBConnection_;          //mysql连接
public:
    DBConnection(spDBPool _DBPool);     //构造函数
    DBConnection(spDBPool _DBPool,upDBSession _DBConnection);     //构造函数
    ~DBConnection();                    //析构函数

    upDBSession GetSession();           //返回原始连接
    mysqlx::Session& operator*() const; //重载*运算符，返回Session引用
};