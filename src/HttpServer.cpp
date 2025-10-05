#include"HttpServer.h"

HttpServer::HttpServer(std::string _ip,uint16_t _port,int _eventLoopNum,int _workNum)
                :tcpServer_(_ip,_port,_eventLoopNum),workThreadPool_(_workNum,"工作线程")                   //构造函数
{
    tcpServer_.SetNewConnectionCB(std::bind(&HttpServer::NewConnection,this,std::placeholders::_1));                 //设置新连接业务回调函数  
    tcpServer_.SetCloseCB(std::bind(&HttpServer::Close,this,std::placeholders::_1));                                 //设置关闭业务回调函数
    tcpServer_.SetMessageCB(std::bind(&HttpServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2)); //设置消息处理函数
    SetGetHandler();
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
    //回显
    printf("接收到：%s\n",_message.c_str());
    HttpConnection httpCon(_connection.get());
    httpHandler handler=nullptr;
    // if(workThreadPool_.size()==0)//无工作线程
    // {
    //     OnMessage(_connection,_message);
    // }else 
    // {
    //     workThreadPool_.AddTask(std::bind(&HttpServer::OnMessage,this,_connection,_message));
    // }

    HttpRequest request;
    if(request.parse(_message)==false)
    {
        //解析失败
        HttpResponse reponse;
        reponse.SetStatusCode(400);
        reponse.SetSatusMessage("解析http失败\n");
        httpCon.send(&reponse);
        return ;
    }

    if(request.GetMethod()=="GET")////////////////////////////////////////////////
    {
        auto it=GetHandler_.find(request.GetPath());
        if(it!=GetHandler_.end())handler=it->second;
    }else 
    {
        //其余暂时均不支持
    }
    if(handler!=nullptr)
    {
        handler();////////////////////////后期使用工作线程处理
        HttpResponse reponse;
        reponse.SetStatusCode(200);
        reponse.SetSatusMessage("OK");
        httpCon.send(&reponse);
    }
    else 
    {
        printf("没有对应处理函数\n");
        HttpResponse reponse;
        reponse.SetStatusCode(404);
        reponse.SetSatusMessage("NOT Found");
        httpCon.send(&reponse);
    }
}
// void HttpServer::OnMessage(spConnection _connection,std::string _message)
// {
//     std::string tmpmessage="回复"+_message;
//     _connection->send(tmpmessage);
// }

// void HttpServer::Stop()
// {
//     workThreadPool_.Stop();
//     tcpServer_.Stop();
// }
void HttpServer::Stop()
{
    workThreadPool_.Stop();
    tcpServer_.Stop();
}
//设置Get处理
void HttpServer::SetGetHandler()
{   
    GetHandler_["/"]=[]
    {
        printf("处理Get请求：/\n");
        printf("处理中……\n");
        sleep(1);
        printf("处理完毕\n");
    };
}