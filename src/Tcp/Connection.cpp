#include"Connection.h"
Connection::Connection(EventLoop* _eventLoop,std::unique_ptr<Socket> _clieSocket)
            :eventLoop_(_eventLoop),
            clieChannel_(new Channel(_clieSocket->fd(),eventLoop_)),
            clieSocket_(std::move(_clieSocket)),
            close_(false)
{
    clieChannel_->UseEt();                                                              //边缘触发
    clieChannel_->EnableReading();                                                      //注册可读
    clieChannel_->SetHandleReadEvent(std::bind(&Connection::HandleReadEvent,this));     //设置读事件处理
    clieChannel_->SetHandleWriteEvent(std::bind(&Connection::HandleWriteEvent,this));   //设置写事件处理
    clieChannel_->SetHandleCloseEvent(std::bind(&Connection::HandleCloseEvent,this));   //绑定关闭连接处理

}

Connection::~Connection()
{
}
void Connection::HandleCloseEvent()
{
    close_=true;
    eventLoop_->DelConnection(fd());//在EventLoop的map中删除connection
    HandleCloseCB(fd());//实际TcpServer移除map对应connection，通知httpServer
}
void Connection::SetHandleCloseEvent(std::function<void(int)> _fun)
{
    HandleCloseCB=_fun;
}

int Connection::fd()
{
    return clieChannel_->fd();
}
void Connection::HandleReadEvent()                                                                 //调用读回调函数
{
    //接收消息并洗出正文（头部+正文）
    static int maxsize=1024;
    char buffer[maxsize];
    while(true)
    {
        memset(buffer,0,sizeof(buffer));
        int recvn=::recv(fd(),buffer,maxsize,0);
        if(recvn>0)//继续接收
        {
            inputBuffer.Append(buffer,recvn);
        }else if(recvn==-1&&(errno==EWOULDBLOCK||errno==EAGAIN))//接收完毕
        {
            std::string message;
            if(inputBuffer.getMessage(message)==false)break;
            lastTime_=TimeStamp::now();//记录上次传输信息时间戳
            HandleMessageCB(shared_from_this(),message);
        }else if(recvn==-1&&errno==EINTR)//信号中断，重试
        {
            continue;
        }else if(recvn==0)//对端关闭
        {
            clieChannel_->remove();
            clieChannel_->HandleCloseEventCB(fd());
            break;
        }
    }
    
}
void Connection::SetHandleMessageEvent(std::function<void(spConnection,std::string)> _fun)                                         //设置读回调函数，TcpServer的处理读函数
{
    HandleMessageCB=_fun;
}
void Connection::HandleWriteEvent()                                                                //处理写事件
{
    int sendn=::send(fd(),outputBuffer.data(),outputBuffer.size(),0);               //尽量一次写满缓冲区，负责出错
    outputBuffer.erase(0,sendn);                                                    //删除outputBuffer
    if(outputBuffer.size()==0)clieChannel_->DisableWriting();                       //发送完毕不再关注写事件
}

//注册读事件
//写入outputBuffer
void Connection::send(std::string _message)                                             
{
    if(close_){printf("已断开连接，不发送\n");return ;}
    std::shared_ptr<std::string>message=std::make_shared<std::string>(_message);
    if(eventLoop_->isLoopThread())
    {
        sendInIOThread(message);
    }else 
    {
        eventLoop_->addTaskLoop(std::bind(&Connection::sendInIOThread,this,message));
    }
    
}
void Connection::sendInIOThread(std::shared_ptr<std::string>message)                                                                  //在IO线程（事件循环线程）中发送
{
    outputBuffer.AppendWithHead(message->data(),message->size());     //写入outputBuffer
    clieChannel_->EnableWriting();
}


bool Connection::timeOut(time_t _now,int sec)//传入现在时间和秒数，判断是否超时
{
    return _now-lastTime_.ToInt()>sec;
}


