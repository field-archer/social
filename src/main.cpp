// #include"HttpServer.h"
#include"MainServer.h"
#include<signal.h>

/*
http服务url的根路径是/home/aaa/Codes/HttpServer/root
下一步：
改进：
1.post，user表加索引
2.令HttpContext只能移动复制和移动构造，替换掉unique_ptr的频繁构造
3.简易http请求，解析路径分出参数，代替消息体传参
3.用std::chrono::system_clock::time_point（封装成类/用c++20的format库）代替string接收post表中的create_time
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