#include"MainServer.h"

MainServer::MainServer(const std::string& _ip,uint16_t _port,int _eventLoopNum,int _workNum,
                //mysql配置
                int _mysqlPort,const std::string& _user,const std::string& _passwd,//端口（3306），用户，密码
                const std::string& _dataBase,size_t _maxSize,size_t _minSize,//数据库，最大总连接数，最小总连接数/最大空闲连接数
                int _idleCheckInterval,int _connectionTimeOut)//空闲连接检查间隔，空闲连接超时时间
                //初始化列表
                :httpServer_(_ip,_port,_eventLoopNum),workThreadPool_(_workNum,"工作线程"),
                mysqlPool_(std::move(std::make_unique<MYSQLConnectionPool>//初始化mysql连接池
                    (_ip,_mysqlPort,_user,_passwd,_dataBase,_maxSize,_minSize,_idleCheckInterval,_connectionTimeOut)))

{
    //初始化连接
    if(mysqlPool_->Init()==false)
    {
        std::cerr<<"初始化连接池有连接创建失败\n";
    }
    //回调函数
    httpServer_.SetSignUp(std::bind(&MainServer::SignUp,this,std::placeholders::_1));
    httpServer_.SetLogIn(std::bind(&MainServer::LogIn,this,std::placeholders::_1));
    //User依赖
    UserDAO* userDAO = new UserDAO(mysqlPool_);
    UserService* userService= new UserService(std::move(std::unique_ptr<UserDAO>(userDAO)));
    UserController userController(std::move(std::unique_ptr<UserService>(userService)));
    userController_=std::move(userController);
}
//析构函数
MainServer::~MainServer()
{

}
//开始函数
void MainServer::Start()
{
    httpServer_.Start();
}
//终止函数
void MainServer::Stop()
{
    workThreadPool_.Stop();
    httpServer_.Stop();
}
//处理注册
void MainServer::SignUp(upContext _context)
{
    userController_.HandleSignUp(std::move(_context));
}
//处理登录
void MainServer::LogIn(upContext _context)
{
    userController_.HandleLogIn(std::move(_context));
}