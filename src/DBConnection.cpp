#include"DBConnection.h"

//构造函数
DBConnection::DBConnection(spDBPool _DBPool):DBPool_(_DBPool)
{

}
//构造函数
DBConnection::DBConnection(spDBPool _DBPool,upDBSession _DBConnection):DBPool_(_DBPool),DBConnection_(std::move(_DBConnection))
{

}
//析构函数
DBConnection::~DBConnection()
{
    DBPool_->ReleaseConnection(DBConnection_);
}
//重载*运算符，返回Session引用
mysqlx::Session& DBConnection::operator*() const
{
    return *DBConnection_;
}