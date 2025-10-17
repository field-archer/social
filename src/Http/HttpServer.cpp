#include"HttpServer.h"
//构造函数        
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
        //构造HttpContext
        upContext context(new HttpContext(request,response,httpCon));
        handler(std::move(context));////////////////////////后期使用工作线程处理
        //move context后response和request均失效
        //发送http响应交给handler()函数
    }
    else 
    {
        printf("没有对应处理函数\n");
        HttpResponse response;
        response.Set404();
        httpCon.send(response);
    }
}
void HttpServer::Stop()
{
    // workThreadPool_.Stop();
    tcpServer_.Stop();
}

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
//设置发帖回调函数
void HttpServer::SetPublishPost(httpHandler _fun)
{
    PostHandler_["/api/post/publish"]=_fun;
}
//设置删帖回调函数
void HttpServer::SetDeletePost(httpHandler _fun)
{
    PostHandler_["/api/post/delete"]=_fun;
}
//设置查看自己的贴子回调函数
void HttpServer::SetCheckMyPosts(httpHandler _fun)
{
    PostHandler_["/api/post/check/my"]=_fun;
}