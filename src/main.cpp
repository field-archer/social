#include"TcpServer.h"
/*
后续可能需要发生改动的地方：
1.通过Channel管理fd
2.Channel增加了处理断开连接的回调函数和对应的设置函数，同时Connection增加了设置Channel回调函数的函数（保持类的层次感），TcpServer增加了删除函数即Channel绑定的断开连接的函数
2.Connection类管理客户端socket//////////////////////////////////////////////重点！！！！！！！！
3.关闭连接时只显示fd，很难优雅的显示ip和port
*/


/*
还需要做：
客户端ip和port是乱码：6(P     �D�:0)已加入连接
Channel的HandleEvent！！！！！！！！！！！！！！！！！！！！！！！！！！
*/

int main()
{
    TcpServer tcpServer("192.168.232.132",8080);
    tcpServer.Start();
    return 0;
}