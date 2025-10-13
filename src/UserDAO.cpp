#include"UserDAO.h"
//构造函数
UserDAO::UserDAO(spDBPool _mysqlPool):mysqlPool_(_mysqlPool)
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
        DBConnection mysqlConnection=mysqlPool_->GetConnection();
        //操作数据库
        (*mysqlConnection).sql("insert into user (name,email,passwd) values(?,?,?)")
                        .bind(_user.GetName(),_user.GetEmail(),_user.GetPasswd()).execute();
        //归还连接
        // mysqlPool_->ReleaseConnection(std::move(mysqlConnection));
        //自动归还连接
    }
    catch(const std::exception& e)
    {
        std::cerr<<"创建用户失败："<<e.what()<<std::endl;
    }
    
}
//登录
bool UserDAO::LogIn(const std::string& _email,const std::string& _passwd)
{
    try
    {
        //获取mysql连接
        DBConnection mysqlConnection=mysqlPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*mysqlConnection).sql("select passwd from user where email = ?").bind(_email).execute();
        mysqlx::Row row=result.fetchOne();
        std::string rePasswd=row[0].get<std::string>();
        return _passwd==rePasswd;
        //归还连接
        // mysqlPool_->ReleaseConnection(std::move(mysqlConnection));
        //自动归还连接
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("数据库连接出错:"+std::string(e.what()));
    }
}