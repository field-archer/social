#pragma once
#include"MYSQLConnectionPool.h"
#include"HttpServer.h"
#include"HttpContext.h"
#include"UserController.h"
#include"PostController.h"

class User;
class UserDAO;
class UserService;
class UserController;
class DBConnection;
class MYSQLConnectionPool;

using spDBPool=std::shared_ptr<MYSQLConnectionPool>;
using upDBSession=std::unique_ptr<mysqlx::Session>;
using wpDBPool=std::weak_ptr<MYSQLConnectionPool>;



class MainServer
{
private:
    HttpServer httpServer_;                                             //HttpServer层
    ThreadPool workThreadPool_;                                         //工作线程
    spDBPool mysqlPool_;                                             //mysql连接池

    UserController userController_;                                     //用户相关功能


    PostDAO postDAO_;                                                   //贴子相关依赖
    PostService postService_;                                           //贴子相关依赖
    PostController postController_;                                     //贴子相关功能



public:   
    MainServer(const std::string& _ip,uint16_t _port,int _eventLoopNum,int _workNum,
                //mysql配置
                int _mysqlPort,const std::string& _user,const std::string& _passwd,//端口（3306），用户，密码
                const std::string& _dataBase,size_t _maxSize,size_t _minSize,//数据库，最大总连接数，最小总连接数/最大空闲连接数
                int _idleCheckInterval,int _connectionTimeOut);//空闲连接检查间隔，空闲连接超时时间
    ~MainServer();

    void Start();                                                   //开始函数
    void Stop();                                                    //终止函数

    //用户服务
    void SignUp(upContext _context);              //处理注册
    void LogIn(upContext _context);               //处理登录


    //贴子服务
    void PublishPost(upContext _context);           //发表贴子
    void DeletePost(upContext _context);            //删除贴子
};