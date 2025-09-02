#include "http.h"

// helper: 从 application/x-www-form-urlencoded 查询串中取 key 的值指针（会把 '&' 改为 '\0'）
static char* _get_param(char* buf, const char* key)
{
    if (!buf || !key) return NULL;
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "%s=", key);
    char* p = strstr(buf, pattern);
    if (!p) return NULL;
    p += strlen(pattern);
    char* end = strchr(p, '&');
    if (end) {
        *end = '\0';
    }
    return p;
}

// 提取 POST body 中 username/type/content（假设 req_buf 已包含并以 '\0' 终止）
void Http::postget()
{
    char* u = _get_param(req_buf, "username");
    char* t = _get_param(req_buf, "type");
    char* c = _get_param(req_buf, "content");

    if (u) username = u;
    if (t) type = atoi(t);
    if (c) text = c;
}

void Http::post_data_handle()
{
    postget();
}

void Http::get_data_handle()
{
    // 根据需要实现 GET 参数的解析
}

int Http::parse_and_process()
{
    if (STRCASECMP(method, "POST") == 0)
    {
        post_data_handle();
    }
    else if (STRCASECMP(method, "GET") == 0)
    {
        get_data_handle();
    }
    return 0;
}

// 逐字读取一行 (处理 CRLF)
// 返回读取的字节数（包含 '\n'），出错/连接关闭返回 -1
int Http::get_line(char* buf)
{
    if (!buf) return -1;
    int i = 0;
    char ch = '\0';
    int ret = 0;

    while (i < SIZE - 1)
    {
        ret = recv(sock, &ch, 1, 0);
        if (ret == 0)
        {
            if (i == 0) return -1; // 对端关闭，且未读到任何数据
            break;
        }
        if (ret < 0)
        {
            return -1; // 错误
        }

        if (ch == '\r')
        {
            // peek 下一个字符是否是 '\n'
            int s = recv(sock, &ch, 1, MSG_PEEK);
            if (s > 0 && ch == '\n')
            {
                // 真的有 '\n'，消费它
                recv(sock, &ch, 1, 0);
                buf[i++] = '\n';
                break;
            }
            else
            {
                buf[i++] = '\n';
                break;
            }
        }

        buf[i++] = ch;
        if (ch == '\n') break;
    }

    buf[i] = '\0';
    return i;
}

void Http::clear_header()
{
    char buf[SIZE];
    while (true)
    {
        int n = get_line(buf);
        if (n <= 0) break;
        if (buf[0] == '\n' || (buf[0] == '\r' && (buf[1] == '\n' || buf[1] == '\0')))
            break;
    }
}

int Http::handle_request()
{
    char line[SIZE] = "";
    int ret = 0;
    content_len = 0;

    if (STRCASECMP(method, "GET") == 0)
    {
        clear_header();
    }
    else
    {
        // 读取 header 并寻找 Content-Length
        while (true)
        {
            ret = get_line(line);
            if (ret <= 0) break;
            if (line[0] == '\n' || (line[0] == '\r' && (line[1] == '\n' || line[1] == '\0')))
                break;

            if (STRNCASECMP(line, "Content-Length", 14) == 0)
            {
                char* p = strchr(line, ':');
                if (p)
                {
                    p++;
                    while (*p && isspace((unsigned char)*p)) p++;
                    long tmp = atol(p);
                    if (tmp < 0) tmp = 0;
                    const long MAX_BODY = (long)sizeof(req_buf) - 1;
                    if (tmp > MAX_BODY) tmp = MAX_BODY;
                    content_len = (int)tmp;
                }
            }
        }
    }

    printf("method:%s\n", method);
    printf("query_string:%s\n", query_string ? query_string : "(null)");
    printf("content_len:%d\n", content_len);

    // 读取 POST body（如果有）
    if (STRCASECMP(method, "POST") == 0 && content_len > 0)
    {
        int total = 0;
        while (total < content_len)
        {
            int r = recv(sock, req_buf + total, content_len - total, 0);
            if (r <= 0) break;
            total += r;
        }
        if (total >= 0)
            req_buf[total] = '\0';
        else
            req_buf[0] = '\0';

        printf("read body bytes: %d\n", total);
        printf("req_buf: %s\n", req_buf);
    }
    else
    {
        req_buf[0] = '\0';
    }

    const char* msg = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(sock, msg, (int)strlen(msg), 0);

    parse_and_process();

    return 0;
}

void Http::GetMethod(int& i, int& k)
{
    int count = GetHead();
    if (count <= 0) return;
    for (; i < count; i++)
    {
        if (isspace((unsigned char)head_buf[i])) break;
        if (k < (int)(sizeof(method) - 1)) method[k++] = head_buf[i];
    }
    method[k] = '\0';
}

int Http::GetHead()
{
    int count = get_line(head_buf);
    return count;
}

void Http::RecvBuf()
{
    recv(sock, del_buf, SIZE, MSG_PEEK);
    printf("接收到数据包\n");
#if 1
    printf("-------------------------------------------------\n");
    printf("%s\n", del_buf);
    printf("-------------------------------------------------\n");
#endif
}

int Http::GetURL(int i)
{
    int t = 0;
    int len = (int)strlen(head_buf);
    for (; i < len; ++i)
    {
        if (isspace((unsigned char)head_buf[i])) break;
        if (head_buf[i] == '?')
        {
            // 终止 url，然后把 ? 后面拷贝到 url 的后面并设置 query_string
            url[t] = '\0';
            ++i;
            int j = t;
            for (; i < len && !isspace((unsigned char)head_buf[i]) && j < SIZE - 1; ++i, ++j)
            {
                url[j] = head_buf[i];
            }
            url[j] = '\0';
            query_string = &url[t];
            return i;
        }
        else
        {
            if (t < SIZE - 1) url[t++] = head_buf[i];
            else break;
        }
    }
    url[t] = '\0';
    query_string = NULL;
    return i;
}

int Http::FixPath()
{
    // 使用 snprintf/strncat 保证安全
    snprintf(path, sizeof(path), "../wwwroot%s", url);
    size_t plen = strlen(path);
    if (plen > 0 && path[plen - 1] == '/')
    {
        strncat(path, "index.html", sizeof(path) - strlen(path) - 1);
    }

    printf("path:%s\n", path);

    struct stat st;
    if (stat(path, &st) == -1)
    {
        printf("文件不存在\n");
        close(sock);
        return -1;
    }
    return 0;
}

int Http::CheckMethod(int& i)
{
    if (STRCASECMP(method, "GET") != 0 && STRCASECMP(method, "POST") != 0)
    {
        printf("method:%s\n", method);
        printf("请求方法错误\n");
        close(sock);
        exit(0);
    }

    if (STRCASECMP(method, "POST") == 0)
    {
        need_handle = 1;
    }

    i = GetURL(i);

    if (STRCASECMP(method, "GET") == 0 && query_string != NULL)
    {
        need_handle = 1;
    }

    return 0;
}

void Http::SkipSpace(int& i)
{
    while (i < SIZE && head_buf[i] && isspace((unsigned char)head_buf[i]))
    {
        i++;
    }
}

void Http::handleChoose()
{
    if (need_handle == 1)
    {
        handle_request();
    }
    else
    {
        clear_header();
    }
}

int Http::Init(int _sock)
{
    sock = _sock;
    memset(del_buf, 0, sizeof(del_buf));
    memset(method, 0, sizeof(method));
    memset(url, 0, sizeof(url));
    query_string = NULL;
    need_handle = 0;
    memset(path, 0, sizeof(path));
    memset(req_buf, 0, sizeof(req_buf));
    content_len = 0;
    type = 0;
    username = NULL;
    text = NULL;
    send_msg_buf = NULL;
    return 0;
}

int Http::handler_msg()
{
    RecvBuf();

    int k = 0;
    int i = 0;
    GetMethod(i, k);
    SkipSpace(i);
    CheckMethod(i);
    handleChoose();

    close(sock);
    return 0;
}

// 发送 HTTP 响应（传入 body）
int Http::send_msg(const char* body)
{
    if (!body) body = "";

    char header[512];
    int body_len = (int)strlen(body);
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        body_len);

    if (header_len < 0) return -1;

    if ((int)send(sock, header, header_len, 0) != header_len) {
        // 发送 header 错误
    }
    if (body_len > 0) {
        if ((int)send(sock, body, body_len, 0) != body_len) {
            // 发送 body 错误
        }
    }
    return 0;
}
