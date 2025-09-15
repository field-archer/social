#include"Connectio.h"
Connection::Connection(std::unique_ptr<EventLoop>& _eventLoop,std::unique_ptr<Socket> _clieSocket)
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
    // delete clieSocket_;                                                                 //析构客户端Socekt
    // delete clieChannel_;                                                                //析构Channel
}
void Connection::HandleCloseEvent()
{
    close_=true;
    HandleCloseCB(fd());
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
        if(recvn>0)
        {
            inputBuffer.Append(buffer,recvn);
        }else if(recvn==-1&&(errno==EWOULDBLOCK||errno==EAGAIN))
        {
            //接收完毕
            uint32_t net_len;
            memcpy(&net_len,inputBuffer.data(),4);
            int len=ntohl(net_len);
            if(inputBuffer.size()<len+4)break;  //不足一份报文，需后续处理
            std::string message(inputBuffer.data()+4,len);
            inputBuffer.erase(0,len+4);
            HandleMessageCB(message);
            
        }else if(recvn==-1&&errno==EINTR)
        {
            //信号终端，重试
            continue;
        }else if(recvn==0)
        {
            //对端关闭
            printf("%d已关闭\n",fd());
            clieChannel_->remove();
            HandleCloseEvent();
            break;
        }
    }
    
}
void Connection::SetHandleMessageEvent(std::function<void(std::string)> _fun)                                         //设置读回调函数，TcpServer的处理读函数
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
//实则不发送
void Connection::send(std::string message)                                             
{
    if(close_){printf("已断开连接，不发送\n");return ;}
    outputBuffer.AppendWithHead(message.data(),message.size());     //写入outputBuffer
    std::cout<<"outPutBuffer:"<<outputBuffer.buff_<<std::endl;
    clieChannel_->EnableWriting();
}
