#include"DBConnection.h"

//构造函数
DBConnection::DBConnection(spDBPool _DBPool):DBPool_(_DBPool)
{

}
//构造函数
DBConnection::DBConnection(spDBPool _DBPool,upDBSession _DBConnection):DBPool_(_DBPool),dbConnection_(std::move(_DBConnection))
{

}
//移动构造函数
DBConnection::DBConnection(DBConnection&& _DBConnection) noexcept:
            DBPool_(std::move(_DBConnection.DBPool_)),dbConnection_(std::move(_DBConnection.dbConnection_))
{
    // 将源对象重置为明确状态
    _DBConnection.DBPool_.reset();
}
//重载移动赋值
DBConnection& DBConnection::operator=(DBConnection&& _DBConnection) noexcept
{
    if(&_DBConnection!=this)
    {
        DBPool_=std::move(_DBConnection.DBPool_);
        dbConnection_=std::move(_DBConnection.dbConnection_);
        // 将源对象重置为明确状态
        _DBConnection.DBPool_.reset();
    }
    return *this;
}
//析构函数
DBConnection::~DBConnection()
{
    //获取临时使用权
    if(auto pool=DBPool_.lock())
    {
        //直接归还资源
        pool->ReleaseConnection(std::move(dbConnection_));
    }
}
//重载*运算符，返回Session引用
mysqlx::Session& DBConnection::operator*() const
{
    return *dbConnection_;
}
//连接是否可用
bool DBConnection::isValid()
{
    try//执行sql语句测试连接有效性
    {
        dbConnection_->sql("select 1").execute();
        return true;
    }catch(...)//连接无效
    {
        return false;
    }
}
