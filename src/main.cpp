#include"HttpServer.h"
#include<signal.h>
/*
大问题：
// 1.前缀4字节
// 2.清除超时客户端
// 3.换教室
6.客户端关闭后对应连接还会超时
// 4.拉肚子
// 5.困
*/

// EchoTcpServer *echoTcpServer;
HttpServer *httpServer;
void Stop(int sig)
{
    printf("收到信号%d\n",sig);
    // echoTcpServer->Stop();
    httpServer->Stop();
    delete httpServer;

    // delete echoTcpServer;
    printf("已安全退出\n");
    exit(0);
}


int main()
{
    signal(SIGINT,Stop);
    signal(SIGTERM,Stop);
    httpServer=new HttpServer("127.0.0.1",8080);
    httpServer->Start();
    // echoTcpServer=new EchoTcpServer("192.168.109.42",8080);
    // echoTcpServer->Start();
    return 0;
}