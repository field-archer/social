#include"HttpServer.h"
//构造函数
// HttpServer::HttpServer(const std::string& _ip,uint16_t _port,int _eventLoopNum,int _workNum,
//                 //mysql配置
//                 int _mysqlPort,const std::string& _user,const std::string& _passwd,//端口（3306），用户，密码
//                 const std::string& _dataBase,size_t _maxSize,size_t _minSize,//数据库，最大总连接数，最小总连接数/最大空闲连接数
//                 int _idleCheckInterval,int _connectionTimeOut)//空闲连接检查间隔，空闲连接超时时间
//                 //初始化列表
//                 :tcpServer_(_ip,_port,_eventLoopNum),workThreadPool_(_workNum,"工作线程"),
//                 mysqlPool_(std::move(std::make_unique<MYSQLConnectionPool>//初始化mysql连接池
//                     (_ip,_mysqlPort,_user,_passwd,_dataBase,_maxSize,_minSize,_idleCheckInterval,_connectionTimeOut)))

// {
//     tcpServer_.SetNewConnectionCB(std::bind(&HttpServer::NewConnection,this,std::placeholders::_1));                 //设置新连接业务回调函数  
//     tcpServer_.SetCloseCB(std::bind(&HttpServer::Close,this,std::placeholders::_1));                                 //设置关闭业务回调函数
//     tcpServer_.SetMessageCB(std::bind(&HttpServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2)); //设置消息处理函数
//     SetGetHandler();        //设置Get方法处理
//     SetPostHandler();       //设置Post方法处理
//     printf("初始化成功！\n");
// }           
HttpServer::HttpServer(const std::string& _ip,uint16_t _port,int _eventLoopNum)//空闲连接检查间隔，空闲连接超时时间
                //初始化列表
                :tcpServer_(_ip,_port,_eventLoopNum)
{
    tcpServer_.SetNewConnectionCB(std::bind(&HttpServer::NewConnection,this,std::placeholders::_1));                 //设置新连接业务回调函数  
    tcpServer_.SetCloseCB(std::bind(&HttpServer::Close,this,std::placeholders::_1));                                 //设置关闭业务回调函数
    tcpServer_.SetMessageCB(std::bind(&HttpServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2)); //设置消息处理函数
    printf("初始化成功！\n");
}                                                                                         
//析构函数
HttpServer::~HttpServer()                     
{

}
void HttpServer::Start()
{
    tcpServer_.Start();
}
//处理新连接业务，仅显示信息
void HttpServer::NewConnection(Socket *clieSocket)                                               
{
    printf("%d（%s:%d）已加入连接\n",clieSocket->fd(),clieSocket->ip().c_str(),clieSocket->port());
}
//关闭业务，仅显示信息
void HttpServer::Close(int _fd)                                                                
{
    printf("%d关闭连接\n",_fd);
}
//处理消息正文，在IO线程中发送
void HttpServer::HandleMessage(spConnection _connection,std::string _message)                 
{
    printf("接收到：%s\n",_message.c_str());
    printf("---------------\n");
    HttpConnection httpCon(_connection.get());
    httpHandler handler=nullptr;
    HttpRequest request;
    
    //解析http
    if(request.parse(_message)==false)
    {
        //解析失败
        HttpResponse reponse;
        reponse.SetStatusCode(400);
        reponse.SetSatusMessage("解析http失败\n");
        httpCon.send(reponse);
        return ;
    }
    // else
    // {
    //     printf("方法:%s\n",request.GetMethod().c_str());
    //     printf("URL:%s\n",request.GetPath().c_str());
    // }

    //根据方法路由http处理器
    if(request.GetMethod()=="GET")
    {
        auto it=GetHandler_.find(request.GetPath());
        if(it!=GetHandler_.end())handler=it->second;
    }else if(request.GetMethod()=="POST")
    {
        auto it=PostHandler_.find(request.GetPath());
        if(it!=PostHandler_.end())handler=it->second;
    }else 
    {
        //其余暂时均不支持
    }

    if(handler!=nullptr)
    {
        printf("有对应处理函数\n");
        HttpResponse response;
        // response.SetStatusCode(200);
        // response.SetSatusMessage("OK");
        //构造HttpContext
        upContext context(new HttpContext(request,response,httpCon));
        handler(std::move(context));////////////////////////后期使用工作线程处理
        //move context后response和request均失效
        //发送http响应交给handler()函数
        // httpCon.send(&reponse);
    }
    else 
    {
        printf("没有对应处理函数\n");
        HttpResponse response;
        response.Set404();
        httpCon.send(response);
    }
}
// void HttpServer::OnMessage(spConnection _connection,std::string _message)
// {
//     std::string tmpmessage="回复"+_message;
//     _connection->send(tmpmessage);
// }

void HttpServer::Stop()
{
    // workThreadPool_.Stop();
    tcpServer_.Stop();
}
//设置Get处理
// void HttpServer::SetGetHandler()
// {   
//     GetHandler_["/"]=[](upContext _aaa)
//     {
//         printf("处理Get请求：/\n");
//         printf("处理中……\n");
//         sleep(1);
//         printf("处理完毕\n");
//     };
// }
//设置post处理
// void HttpServer::SetPostHandler()
// {
//     // PostHandler_["/api/user"]=std::bind(&HttpServer::PostUser,this);
//     // SetSignUp
// }
//用户注册函数/api/user
//用户名(name)，邮箱(email)，密码(passwd)
// void HttpServer::PostUser()
// {
    
//     try
//     {
//         spmysqlConnection mysqlConnection=mysqlPool_->GetConnection();
//         mysqlConnection->sql("insert into user values(?,?,?);").bind().execute();
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
    
// }   
//设置用户注册回调函数
void HttpServer::SetSignUp(std::function<void(upContext)> _fun)
{
    PostHandler_["/api/user/signup"]=_fun;
}
//设置用户登录回调函数
void HttpServer::SetLogIn(std::function<void(upContext)> _fun)
{
    PostHandler_["/api/user/login"]=_fun;
}
/*
POST /api/users HTTP/1.1
Host: your-social-platform.com
Content-Type: application/json
User-Agent: Mozilla/5.0 (YourApp/1.0)

{
  "username": "alice123",
  "password": "aSecurePassword123!",
  "email": "alice@example.com",
  "display_name": "Alice"
}
*/