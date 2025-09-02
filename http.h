#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h> // for strncasecmp, strcasecmp

#define SIZE 4096

// 跨平台小宏（此头为 Linux 版本，映射到 POSIX 名）
#define STRNCASECMP strncasecmp
#define STRCASECMP  strcasecmp

#define LOGIN 1
#define LOGOUT 2
#define MESSAGE 3
#define GETHISTORY 4

class Http
{
private:
    int sock;                           // 连接客户端的套接字
    char head_buf[SIZE];                // 请求头部缓冲区
    char del_buf[SIZE];                 // 存放接收到的数据包
    char method[32];                    // 储存请求方法的容器
    char url[SIZE];                     // 存放资源路径的容器
    char* query_string;                 // 指向URL中是否有要处理的数据，如果有，指向起始地址
    int need_handle;                    // 是否需要手动处理请求,1表示需要，0表示不需要
    char req_buf[4096];                 // 存放请求报文的容器
    int content_len;                    // 请求报文中的Content-Length字段的值
    char path[SIZE];

public:
    int type;                           // 1表示登录，2表示登出，3表示消息，4表示获取历史消息
    char* username;                     // 登录用户名（指向 req_buf 内部或由你 strdup）
    char* text;                         //接收报文body的容器，即用户发送的消息（指向 req_buf）
    char* send_msg_buf;                 //发送的消息（注意生命周期）
private:
    void postget();

    void handleChoose();
    // 读取请求相关
    int get_line(char* buf);
    void clear_header();
    int handle_request();

    void GetMethod(int& i, int& k);
    int GetHead();

    void RecvBuf();
    int GetURL(int i);
    int FixPath();

    int CheckMethod(int& i);
    void SkipSpace(int &i);

    void post_data_handle();
    void get_data_handle();

    int parse_and_process();

public:
    int handler_msg();
    int send_msg(const char* msg);
    int Init(int _sock);
};
