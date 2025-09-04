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
    void RecvBuf();                     // 接收数据包   
    void GetMethod(int& i, int& k);     // 获取请求方法
    int GetHead();                      // 获取请求头部
    void SkipSpace(int &i);             // 跳过空白字符
    int CheckMethod(int& i);            // 检查请求方法是否合法，并判断是否需要手动处理
    int GetURL(int i);                  // 获取资源路径
    void handleChoose();                // 处理选择请求
    int handle_request();               // 处理请求
    void clear_header();                // 清空请求头部
    int parse_and_process();            // 解析请求报文并处理
    void post_data_handle();            // 处理POST请求的数据
    void postget();                     // 处理POST请求的数据
    // 读取请求相关
    
    //----------------------------------
    void get_data_handle();             // 处理GET请求的数据
    int FixPath();                      // 获取资源路径（暂时用不到）
    int get_line(char* buf);            // 读取一行数据
public:
    int Init(int _sock);                 // 初始化
    int handler_msg();                   // 处理消息请求
    int send_msg( char *uname ,int type , char* msg);       // 拼接消息
};  
