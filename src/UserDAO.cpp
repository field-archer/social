#include"UserDAO.h"
//构造函数
UserDAO::UserDAO(spMYSQLPOOL _mysqlPool):mysqlPool_(_mysqlPool)
{

}
//析构函数
UserDAO::~UserDAO()
{

}

//创建用户
void UserDAO::CreateUser(const User& _user)
{
    try
    {
        //获取mysql连接
        spmysqlConnection mysqlConnection=mysqlPool_->GetConnection();
        //操作数据库
        mysqlConnection->sql("insert into user (name,email,passwd) values(?,?,?)")
                        .bind(_user.GetName(),_user.GetEmail(),_user.GetPasswd()).execute();
        //归还连接
        mysqlPool_->ReleaseConnection(mysqlConnection);
    }
    catch(const std::exception& e)
    {
        std::cerr<<"创建用户失败："<<e.what()<<std::endl;
    }
    
}