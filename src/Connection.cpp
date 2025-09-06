#include"Connectio.h"
Connection::Connection(int _fd,EventLoop *_eventLoop,Socket *_clieSocket)
{
    eventLoop_=_eventLoop;                                                              //已有eventLoop
    clieSocket_=_clieSocket;                                                            //管理本不该有的客户端Socekt
    clieChannel_=new Channel(_fd,_eventLoop);                                           //新Channel
    clieChannel_->UseEt();                                                              //边缘触发
    clieChannel_->EnableReading();                                                      //注册可读
    // clieChannel_->SetHandleReadEvent(std::bind(&Connection::DataChange,this));       //绑定读事件处理：Channel的数据处理

    clieChannel_->SetHandleLogInEvent(std::bind(&Connection::HandleLogInEvent,this,std::placeholders::_1));     //绑定用户登录处理
    clieChannel_->SetHandleLogOutEvent(std::bind(&Connection::HandleLogOutEvent,this));                         //绑定登出处理
    clieChannel_->SetHandleMessageEvent(std::bind(&Connection::HandleMessageEvent,this,std::placeholders::_1)); //绑定消息处理
    clieChannel_->SetHandleCloseEvent(std::bind(&Connection::HandleCloseEvent,this));                           //绑定关闭连接处理
    // clieChannel_->SetHandleWriteEvent(std::bind(&Connection::WriteCallBack,this));    //绑定用户可写处理，负责数据发送
}

Connection::~Connection()
{
    delete clieSocket_;                                                                 //析构本不该有的客户端Socekt
    delete clieChannel_;                                                                //析构Channel
}
// void Connection::SetCloseCB(std::function<void()> _fun)                       //设置channel处理关闭事件的函数
// {
//     CloseCB_=_fun;
// }
// void Connection::SetLogInCB(std::function<void(char *)> _fun)                    //设置Channel处理登陆事件的回调函数
// {
//     LogInCB_=_fun;
// }
// void Connection::SetMessageCB(std::function<void(Connection*,std::string)> _fun)                       //设置处理用户消息的回调函数
// {
//     MessageCB_=_fun;
// }
void Connection::HandleLogInEvent(std::string _name)
{
    name_=_name;
    if(name_=="")name_="名字不可以为空嗷！";
}
void Connection::HandleLogOutEvent()
{
    HandleLogOutEventCB();
}
void Connection::SetHandleLogOutEvent(std::function<void()> _fun)
{
    HandleLogOutEventCB=_fun;
}
void Connection::HandleMessageEvent(std::string message)
{
    HandleMessageEventCB(message);
}
void Connection::SetHandleMessageEvent(std::function<void(std::string message)> _fun)
{
    HandleMessageEventCB=_fun;
}
void Connection::HandleCloseEvent()
{
    HandleCloseCB();
}
void Connection::SetHandleCloseEvent(std::function<void()> _fun)
{
    HandleCloseCB=_fun;
}


// void Connection::SetReadCB(std::function<void()> _ReadCB)
// {
//     ReadCB_=_ReadCB;
// }
void Connection::HandleRead()
{
    
}
// void Connection::CloseCB()
// {
//     CloseCB_();
// }
// void Connection::DataChange()
// {
//     static const int maxsize=1024;
//     char buff[maxsize];
//     while(true)
//     {
//         int recvn=::recv(fd(),buff,maxsize-1,0);//bufsize-1保证有'\0'位置
//         if(recvn==-1)                              //出错
//         {
//             if(errno==EAGAIN||errno==EWOULDBLOCK)               //数据读取完毕
//             {
//                 while(true)
//                 {
//                     int len;
//                     memcpy(&len,inputBuffer.data(),4);          //获取报文长度
//                     if(inputBuffer.size()<len+4) break;         //数据读完且剩余数据不够一组！！！！！！！后续处理这种情况

//                     std::string message(inputBuffer.data()+4,len);  //获取报文正文
//                     inputBuffer.erase(0,len+4);                     //删除该报文信息（头部+内容）

//                     printf("%d发来消息：%s\n",fd(),message.c_str());
                    
//                    MessageCB_(this,message);                    //将报文正文交给Tcpserver处理
//                 }
//                 break;

//             }else if(errno==EINTR)
//             {
//                 printf("信号终端，重新接收\n");
//                 continue;
//             }else if(errno==ENOTCONN)
//             {
//                 printf("fd（%d）未连接\n",fd());
//                 break;
//             }else if(errno==EBADF)
//             {
//                 printf("无效fd(%d)\n",fd());
//                 break;
//             }else 
//             {
//                 printf("文件%s的%d行的[%s]函数出错,fd=%d\n", __FILE__, __LINE__, __func__,fd());
//                 perror("接收客户端的数据出错:");
//                 break;
//             }
//         }else if(recvn==0)                          //对方关闭
//         {
//             printf("对方已关闭\n");
//             CloseCB();
//             break;
//         }else                                       //正常 
//         {   
//             inputBuffer.Append(buff,recvn);         //持续接收数据到缓冲区
//         }
//     }
// }
int Connection::fd()
{
    return clieChannel_->fd();
}
void Connection::send(std::string message)          //发送数据
{
    clieChannel_->Send(message);
}
void Connection::WriteCallBack()                                           //发送数据函数，供Channel回调
{
    int writen=::send(fd(),outputBuffer.data(),outputBuffer.size(),0);
    if(writen>0)outputBuffer.erase(0,writen);
    if(outputBuffer.size()==0)clieChannel_->DisableWriting();
}
