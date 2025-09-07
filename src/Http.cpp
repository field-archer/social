// http_text_tool.cpp
// #include "http_text_tool.h"
#include"Http.h"

Http::Http()
{
    reset();
}

void Http::reset()
{
    buffer.clear();
    method.clear();
    url.clear();
    query_string.clear();
    content_len = 0;
    param_map.clear();
    username.clear();
    text.clear();
    type = 0;
}

const string &Http::get_method() const { return method; }
const string &Http::get_url() const { return url; }
const string &Http::get_query_string() const { return query_string; }
int Http::get_content_length() const { return content_len; }
const string &Http::get_username() const { return username; }
const string &Http::get_text() const { return text; }
const map<string, string> &Http::params() const { return param_map; }

string Http::tolower_copy(const string &s)
{
    std::string t = s;
    std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c)
                   { return std::tolower(c); });
    return t;
}

vector<string> Http::split(const string &s, char delim)
{
    std::vector<std::string> out;
    std::string cur;
    std::istringstream iss(s);
    while (std::getline(iss, cur, delim))
        out.push_back(cur);
    return out;
}

string Http::url_decode(const string &s)
{
    std::string ret;
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '+')
        {
            ret.push_back(' ');
        }
        else if (c == '%' && i + 2 < s.size())
        {
            char h1 = s[i + 1];
            char h2 = s[i + 2];
            auto hexval = [](char ch) -> int
            {
                if (ch >= '0' && ch <= '9')
                    return ch - '0';
                if (ch >= 'A' && ch <= 'F')
                    return ch - 'A' + 10;
                if (ch >= 'a' && ch <= 'f')
                    return ch - 'a' + 10;
                return 0;
            };
            char decoded = (char)((hexval(h1) << 4) | hexval(h2));
            ret.push_back(decoded);
            i += 2;
        }
        else
        {
            ret.push_back(c);
        }
    }
    return ret;
}

void Http::parse_query_string_to_map(const string &qs)
{
    param_map.clear();
    if (qs.empty())
        return;
    auto parts = split(qs, '&');
    for (auto &p : parts)
    {
        size_t pos = p.find('=');
        if (pos == std::string::npos)
            continue;
        std::string k = p.substr(0, pos);
        std::string v = p.substr(pos + 1);
        param_map[k] = url_decode(v);
    }
}

void Http::parse_urlencoded_body(const string &body)
{
    parse_query_string_to_map(body);
    // 填充常用字段（username,type,content）
    auto it = param_map.find("username");
    if (it != param_map.end())
        username = it->second;
    it = param_map.find("content");
    if (it != param_map.end())
        text = it->second;
    it = param_map.find("type");
    if (it != param_map.end())
        type = atoi(it->second.c_str());
}

void Http::parse_request_line_and_headers(const string &header_block)
{
    // header_block: 包含 request-line 和 headers（不含 \r\n\r\n）
    // 分成行
    auto lines = split(header_block, '\n');
    if (lines.empty())
        return;
    // request line
    // 形如: "POST /path?x=1 HTTP/1.1"
    {
        istringstream iss(lines[0]);
        iss >> method;
        string fullurl;
        iss >> fullurl;
        // 分离 url 和 query_string
        size_t qpos = fullurl.find('?');
        if (qpos != string::npos)
        {
            url = fullurl.substr(0, qpos);
            query_string = fullurl.substr(qpos + 1);
        }
        else
        {
            url = fullurl;
            query_string.clear();
        }
    }

    // headers
    content_len = 0;
    for (size_t i = 1; i < lines.size(); ++i)
    {
        string line = lines[i];
        // trim CR/WHITESPACE
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();
        if (line.empty())
            continue;
        size_t colon = line.find(':');
        if (colon == string::npos)
            continue;
        string name = tolower_copy(line.substr(0, colon));
        string value = line.substr(colon + 1);
        // 去除前后的空格
        size_t p = 0;
        while (p < value.size() && isspace((unsigned char)value[p]))
            ++p;
        if (p)
            value.erase(0, p);
        if (name == "content-length")
        {
            content_len = atoi(value.c_str());
        }
        
        //拓展解析
    }

    // 如果是 GET 且有 query_string，则解析 query params
    if (tolower_copy(method) == "get" && !query_string.empty())
    {
        parse_query_string_to_map(query_string);
        auto it = param_map.find("username");
        if (it != param_map.end())
            username = it->second;
        it = param_map.find("content");
        if (it != param_map.end())
            text = it->second;
        it = param_map.find("type");
        if (it != param_map.end())
            type = atoi(it->second.c_str());
    }
}

void Http::parse_body_if_ready()
{
    // buffer currently holds header+body (或只是 header if incomplete)
    // 找到 header 与 body 的分界处
    const char *hdr_sep = nullptr;
    size_t pos = string::npos;
    // 首先尝试 \r\n\r\n
    pos = buffer.find("\r\n\r\n");
    size_t sep_len = 4;
    if (pos == string::npos)
    {
        // 回退尝试 \n\n
        pos = buffer.find("\n\n");
        sep_len = 2;
    }
    if (pos == string::npos)
    {
        // header 未完整
        return;
    }

    string header_block = buffer.substr(0, pos);
    parse_request_line_and_headers(header_block);

    // body start
    size_t body_start = pos + sep_len;
    size_t available_body = buffer.size() > body_start ? buffer.size() - body_start : 0;

    // 如果 content_len > 0，则确保 body 完整
    if (content_len > 0)
    {
        if ((int)available_body < content_len)
        {
            // 需要更多数据
            return;
        }
        std::string body = buffer.substr(body_start, content_len);
        // 仅支持 application/x-www-form-urlencoded
        parse_urlencoded_body(body);
        // 可以在这里把 buffer 保留多余的数据（pipeline）或清空
        // 这里选择清空 buffer（外部如果要 pipeline，请调整）
        buffer.clear();
    }
    else
    {
        // GET 或无 body： nothing to parse for body
        // 若 URL 中带 query_string，已经在 parse_request_line_and_headers 中解析
        buffer.clear();
    }
}

// parse_raw: incremental parsing
int Http::parse_raw(const char *buf, int len)
{
    if (!buf || len <= 0)
        return -1;
    // 用buffer存储传入的数据
    buffer.append(buf, buf + len);

    // 判断 header 是否完整（存在 \r\n\r\n 或 \n\n）
    size_t header_end = buffer.find("\r\n\r\n");
    if (header_end == string::npos)
        header_end = buffer.find("\n\n");

    if (header_end == string::npos)
    {
        // header 未完整，继续等待更多数据
        return 1;
    }

    // 先解析 header（并获得 content_len），再判断 body 是否完整
    std::string header_block = buffer.substr(0, header_end);
    parse_request_line_and_headers(header_block);

    // 判断 body 是否完整
    size_t body_start = header_end + ((buffer.find("\r\n\r\n") != string::npos) ? 4 : 2);
    size_t available_body = buffer.size() > body_start ? buffer.size() - body_start : 0;
    if (content_len > 0)
    {
        if ((int)available_body < content_len)
        {
            // 需要更多数据
            return 1;
        }
    }
    // 到这里 header + body（如果有）都在 buffer 中，执行解析
    parse_body_if_ready();
    return 0;
}

string Http::build_response(const char *uname, int type, const char *msg)
{
    if (!uname)
        uname = "";
    if (!msg)
        msg = "";
    ostringstream body_ss;
    // body 使用 application/x-www-form-urlencoded 格式
    // 注意需要对 uname 和 msg 做 url-encoding，这里简单替换空格为 '+' 并不过度编码
    auto urlencode_simple = [](const string &s) -> string
    {
        ostringstream o;
        for (unsigned char c : s)
        {
            if (isalnum(c) || c == '.' || c == '-' || c == '_' || c == '~')
                o << c;
            else if (c == ' ')
                o << '+';
            else
            {
                static const char *hex = "0123456789ABCDEF";
                o << '%' << hex[c >> 4] << hex[c & 0xF];
            }
        }
        return o.str();
    };

    body_ss << "username=" << urlencode_simple(uname)
            << "&type=" << type
            << "&content=" << urlencode_simple(msg);

    string body = body_ss.str();
    ostringstream header_ss;
    header_ss << "HTTP/1.1 200 OK\r\n"
              << "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n"
              << "Content-Length: " << body.size() << "\r\n"
              << "Connection: close\r\n"
              << "\r\n"
              << body;
    return header_ss.str();
}
