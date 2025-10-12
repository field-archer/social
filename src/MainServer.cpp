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
    // httpServer_.SetNewConnectionCB(std::bind(&HttpServer::NewConnection,this,std::placeholders::_1));                 //设置新连接业务回调函数  
    // httpServer_.SetCloseCB(std::bind(&HttpServer::Close,this,std::placeholders::_1));                                 //设置关闭业务回调函数
    // httpServer_.SetMessageCB(std::bind(&HttpServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2)); //设置消息处理函数
    httpServer_.SetSignUp(std::bind(&MainServer::SignUp,this,std::placeholders::_1));
    httpServer_.SetLogIn(std::bind(&MainServer::LogIn,this,std::placeholders::_1));
    printf("初始化成功！\n");
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
    //获取姓名（name），邮箱（email），密码（passwd）
    std::string name="",email="",passwd="";
    
    try
    {
        //构造User结构体
        User newUser(_context->GetRequestBody());
        //注册用户
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    if(_context->GetRequestBody("name",name)&&_context->GetRequestBody("email",email)&&_context->GetRequestBody("passwd",passwd))
    {
        printf("获取用户信息成功,name=%s,email=%s,passwd=%s\n",name.c_str(),email.c_str(),passwd.c_str());
        try
        {
            //获取mysql连接
            spmysqlConnection mysqlConnection=mysqlPool_->GetConnection();
            mysqlConnection->sql("insert into user (name,email,passwd) values (?,?,?)")
                            .bind(name,email,passwd).execute();
        }catch(const std::exception& e)
        {
            std::cerr<<"注册用户失败:"+std::string(e.what())<<std::endl;
        }
        _context->Send200();
    }else//消息体无对应数据
    {
        printf("获取用户信息失败\n");
        _context->Send404();
        _context->SendWithoutBOdy(400,"数据不全");
    }
}
//处理登录
void MainServer::LogIn(upContext _context)
{
    //获取邮箱（email），密码（passwd）
    std::string email="",passwd="";
    if(_context->GetRequestBody("email",email)&&_context->GetRequestBody("passwd",passwd))
    {
        try
        {
            //获取mysql连接
            spmysqlConnection mysqlConnection=mysqlPool_->GetConnection();
            mysqlx::SqlResult result=mysqlConnection->sql("select passwd from user where email=?")
                                        .bind(email).execute();
            const mysqlx::Row& row=result.fetchOne();
            std::string rePasswd=row[0].get<std::string>();
            if(rePasswd==passwd)
            {
                //密码正确
                _context->SendWithoutBOdy(200,"密码正确");

            }else
            {
                //密码错误
                _context->SendWithoutBOdy(401,"密码错误");
            }
        }catch(const std::exception& e)
        {
            std::cerr<<"注册用户失败:"+std::string(e.what())<<std::endl;
        }
    }else//消息体无对应数据
    {
        _context->Send404();
    }
}