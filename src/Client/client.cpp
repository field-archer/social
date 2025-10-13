#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sstream>
#include <map>
#include <iomanip>
#include<nlohmann/json.hpp>

using json = nlohmann::json;

class HttpClient {
private:
    int sock;
    struct sockaddr_in server_addr;

public:
    HttpClient(const std::string& address, int port) {
        // 创建socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            throw std::runtime_error("Socket creation failed");
        }

        // 设置服务器地址
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, address.c_str(), &server_addr.sin_addr) <= 0) {
            close(sock);
            throw std::runtime_error("Invalid address");
        }
    }

    ~HttpClient() {
        if (sock >= 0) {
            close(sock);
        }
    }

    bool connect() {
        return ::connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) >= 0;
    }

    std::string sendRequest(const std::string& request) {
        // 添加4字节长度前缀
        uint32_t len = htonl(static_cast<uint32_t>(request.size()));
        std::string prefixed_request;
        prefixed_request.resize(4 + request.size());
        
        memcpy(&prefixed_request[0], &len, 4);
        memcpy(&prefixed_request[4], request.c_str(), request.size());

        // 发送请求
        if (send(sock, prefixed_request.c_str(), prefixed_request.size(), 0) < 0) {
            return "Send failed";
        }

        // 接收响应长度前缀
        uint32_t response_len_net;
        int bytes_read = recv(sock, &response_len_net, 4, 0);
        if (bytes_read != 4) {
            return "Failed to read length prefix";
        }
        uint32_t response_len = ntohl(response_len_net);

        // 接收响应主体
        std::string response(response_len, '\0');
        bytes_read = recv(sock, &response[0], response_len, 0);
        if (bytes_read != static_cast<int>(response_len)) {
            return "Incomplete response received";
        }

        return response;
    }
};

int main() {
    try {
        HttpClient client("192.168.38.121", 8080);
        
        if (!client.connect()) {
            std::cerr << "Connection failed" << std::endl;
            return 1;
        }

        int choice;
        std::cout << "请选择操作: 1(注册) 或 2(登录): ";
        std::cin >> choice;
        std::cin.ignore(); // 清除输入缓冲区

        std::string path;
        json request_data; // 使用 nlohmann::json 对象
        
        if (choice == 1) {
            // 注册
            std::string name, email, passwd;
            std::cout << "请输入用户名: ";
            std::getline(std::cin, name);
            std::cout << "请输入邮箱: ";
            std::getline(std::cin, email);
            std::cout << "请输入密码: ";
            std::getline(std::cin, passwd);
            
            request_data["name"] = name;
            request_data["email"] = email;
            request_data["passwd"] = passwd;
            
            path = "/api/user/signup";
            
        } else if (choice == 2) {
            // 登录
            std::string email, passwd;
            std::cout << "请输入邮箱: ";
            std::getline(std::cin, email);
            std::cout << "请输入密码: ";
            std::getline(std::cin, passwd);
            
            request_data["email"] = email;
            request_data["passwd"] = passwd;
            
            path = "/api/user/login";
            
        } else {
            std::cerr << "无效的选择" << std::endl;
            return 1;
        }

        // 将 JSON 对象转换为字符串
        std::string message_body = request_data.dump();

        // 构造HTTP请求
        std::string http_request = 
            "POST " + path + " HTTP/1.1\r\n"
            "Host: localhost:8080\r\n"
            "Connection: close\r\n"
            "User-Agent: C++HttpClient/1.0\r\n"
            "Content-Type: application/json\r\n"  // 修改为 application/json
            "Content-Length: " + std::to_string(message_body.size()) + "\r\n"
            "\r\n" +
            message_body;

        std::cout << "发送的HTTP请求:\n" << http_request << std::endl;
        std::cout << "----------------------" << std::endl;

        // 发送请求并获取响应
        std::string response = client.sendRequest(http_request);
        
        // 解析HTTP响应
        size_t pos = response.find("\r\n");
        std::string status_line = response.substr(0, pos);
        
        size_t body_pos = response.find("\r\n\r\n");
        std::string headers = response.substr(pos + 2, body_pos - pos - 2);
        std::string body = response.substr(body_pos + 4);
        
        // 提取状态码
        size_t code_pos = status_line.find(' ');
        std::string status_code = status_line.substr(code_pos + 1, 3);
        
        std::cout << "响应状态: " << status_line << std::endl;
        std::cout << "响应码: " << status_code << std::endl;
        std::cout << "响应头: " << std::endl << headers << std::endl;
        
        // 尝试解析响应体为 JSON
        try {
            json response_json = json::parse(body);
            std::cout << "响应体 (JSON格式): " << std::endl << response_json.dump(4) << std::endl;
            
            // 提取并打印常见的 JSON 字段
            if (response_json.contains("code")) {
                std::cout << "业务状态码: " << response_json["code"] << std::endl;
            }
            if (response_json.contains("message")) {
                std::cout << "消息: " << response_json["message"] << std::endl;
            }
            if (response_json.contains("data")) {
                std::cout << "数据: " << response_json["data"] << std::endl;
            }
        } catch (const json::parse_error& e) {
            // 如果解析失败，按普通文本输出
            std::cout << "响应体 (文本格式): " << std::endl << body << std::endl;
            std::cerr << "JSON解析错误: " << e.what() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}