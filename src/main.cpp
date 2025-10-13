// #include"HttpServer.h"
#include"MainServer.h"
#include<signal.h>

/*
http服务url的根路径是/home/aaa/Codes/HttpServer/root
下一步：
在MainServer的构造函数中搞定User系列的依赖
*/

// HttpServer *httpServer;
MainServer *mainServer;

void Stop(int sig)
{
    printf("收到信号%d\n",sig);
    mainServer->Stop();
    delete mainServer;

    printf("已安全退出\n");
    exit(0);
}


int main()
{
    signal(SIGINT,Stop);
    signal(SIGTERM,Stop);
    mainServer=new MainServer("192.168.38.121",8080,2,2,33060,"aaa","1234qwer","test",10,3,30,300);
    mainServer->Start();
    return 0;
}