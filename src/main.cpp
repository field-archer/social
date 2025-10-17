// #include"HttpServer.h"
#include"MainServer.h"
#include<signal.h>

/*
http服务url的根路径是/home/aaa/Codes/HttpServer/root
下一步：
httpContext将设置状态码、状态信息、常用头封装，postController和userController中使用该封装
改进：
1.post，user表加索引
2.用std::chrono::system_clock::time_point（封装成类/用c++20的format库）代替string接收post表中的create_time
3.令HttpContext只能移动复制和移动构造，替换掉unique_ptr的频繁构造
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