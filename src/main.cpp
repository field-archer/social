#include"EchoTcpServer.h"
#include<signal.h>
/*
逆天IO线程池，notify_all，没一个醒的，还能秒过join
*/

EchoTcpServer *echoTcpServer;
void Stop(int sig)
{
    printf("收到信号%d\n",sig);
    echoTcpServer->Stop();

    delete echoTcpServer;
    printf("已安全退出\n");
    exit(0);
}


int main()
{
    signal(SIGINT,Stop);
    signal(SIGTERM,Stop);

    echoTcpServer=new EchoTcpServer("192.168.109.42",8080);
    echoTcpServer->Start();
    return 0;
}