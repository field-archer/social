// #include"TcpServer.h"
#include"EchoTcpServer.h"
/*
后续可能需要发生改动的地方：
*/


/*
还需要做：
*/

int main()
{
    // TcpServer tcpServer("192.168.232.135",8080);
    // tcpServer.Start();
    EchoTcpServer echoTcpServer("192.168.109.42",8080,10,4);
    echoTcpServer.Start();
    return 0;
}