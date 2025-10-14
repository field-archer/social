#include"UserDAO.h"
//构造函数
UserDAO::UserDAO(spDBPool _mysqlPool):mysqlPool_(_mysqlPool)
{

}
//析构函数
UserDAO::~UserDAO()
{

}

//创建用户，返回userId(0表失败),_user内设置userId
int UserDAO::CreateUser(User& _user)
{
    try
    {
        //获取mysql连接
        DBConnection mysqlConnection=mysqlPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*mysqlConnection).sql("insert into user (name,email,passwd) values(?,?,?)")
                        .bind(_user.GetName(),_user.GetEmail(),_user.GetPasswd()).execute();
        int id=result.getAutoIncrementValue();//自增主键id
        _user.SetId(id);
        return id;
        //自动归还连接
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("创建用户失败"+std::string(e.what()));
    }
    
}
//登录，返回userId(0表失败)
int UserDAO::LogIn(const std::string& _email,const std::string& _passwd)
{
    try
    {
        //获取mysql连接
        DBConnection mysqlConnection=mysqlPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*mysqlConnection).sql("select id,passwd from user where email = ?")
                            .bind(_email).execute();
        mysqlx::Row row=result.fetchOne();
        int id=row[0].get<int>();
        std::string rePasswd=row[1].get<std::string>();
        if(_passwd!=rePasswd)return 0;
        else return id;
        //自动归还连接
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("用户登录出错:"+std::string(e.what()));
    }
}
//根据userId判断用户是否存在
bool UserDAO::exist(int userId)
{
    try
    {
        //获取mysql连接
        DBConnection mysqlConnection=mysqlPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*mysqlConnection).sql("select name from user where id = ?")
                            .bind(userId).execute();
        //自动归还连接
        return result.count()==1;
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("判断用户是否存在出错:"+std::string(e.what()));
    }
}
