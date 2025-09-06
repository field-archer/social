// http_text_tool.h
#pragma once

#include <string>
#include <map>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <vector>
using namespace std;
#define LOGIN 1
#define LOGOUT 2
#define MESSAGE 3
#define GETHISTORY 4

class Http
{
public:
    Http();
    void reset(); // 重置内部状态

    // -------- 解析接口（外部接收原始报文后调用） ----------
    // 将外部 recv 得到的原始数据（可能是头+body，也可能不完整）交给 Http 去解析
    // buf：原始数据指针
    // len：原始数据长度
    // 返回值：
    //   0 = 完整解析成功（已填充 method, url, username, text, type 等成员）
    //   1 = 数据不完整，需要更多字节（外部继续 recv 并 append，再调用 parse_raw）1
    //  -1 = 解析出错
    int parse_raw(const char *buf, int len);

    // 访问解析结果
    const string &get_method() const;
    const string &get_url() const;
    const string &get_query_string() const; // 如果有
    int get_content_length() const;

    // 业务字段（POST/GET 解析后填充）
    int type; // 1/2/3/4
    const string &get_username() const;
    const string &get_text() const;

    // 获取解析得到的所有表单/查询参数
    const map<string, string> &params() const;

    // -------- 组装接口（外部想要 send 数据时调用） ----------
    // 根据业务字段组装一个完整的 HTTP/1.1 响应报文（header+body）
    // 返回字符串（外部直接 send(resp.data(), resp.size())）
    static string build_response(const char *uname, int type, const char *msg);

    // -------- 可选工具 ----------
    // URL decode（例如把 '+' -> ' '，把 %xx 解码）
    static string url_decode(const string &s);

private:
    string buffer; // 用于 incremental parse：保存传入的原始数据（可能不完整）
    string method;
    string url;
    string query_string;
    int content_len;
    map<string, string> param_map;
    string username;
    string text;

    // 内部 helper
    static string tolower_copy(const string &s);
    static vector<string> split(const string &s, char delim);
    void parse_request_line_and_headers(const string &header_block);
    void parse_body_if_ready();
    void parse_urlencoded_body(const string &body);
    void parse_query_string_to_map(const string &qs);
};
// 使用案例：
//// recv_loop.cpp 伪示例
// Http h;
// h.reset();
// char netbuf[4096];
// int r = recv(client_sock, netbuf, sizeof(netbuf), 0);
// if (r > 0) {
//     int rc = h.parse_raw(netbuf, r);
//     if (rc == 0) {
//         // 解析成功，可以读取字段
//         std::string user = h.get_username();
//         std::string text = h.get_text();
//         int type = h.type;
//         // 处理业务...
//     } else if (rc == 1) {
//         // 数据不完整，继续从 socket 读取并 append（再次调用 parse_raw）
//     } else {
//         // 解析错误
//     }
// }
// // 发送示例
// std::string resp = Http::build_response("alice", MESSAGE, "hello world");
// send(client_sock, resp.data(), (int)resp.size(), 0);
