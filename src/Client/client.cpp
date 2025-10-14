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
#include <nlohmann/json.hpp>

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

// 解析HTTP响应并返回状态码和JSON响应体
std::pair<int, json> parseHttpResponse(const std::string& response) {
    size_t pos = response.find("\r\n");
    std::string status_line = response.substr(0, pos);
    
    size_t code_pos = status_line.find(' ');
    int status_code = std::stoi(status_line.substr(code_pos + 1, 3));
    
    size_t body_pos = response.find("\r\n\r\n");
    std::string body = response.substr(body_pos + 4);
    
    json response_json;
    try {
        response_json = json::parse(body);
    } catch (const json::parse_error& e) {
        // 如果解析失败，创建一个包含错误信息的JSON对象
        response_json = {{"error", "Failed to parse JSON response"}, {"raw_response", body}};
    }
    
    return {status_code, response_json};
}

// 发送HTTP请求并显示响应
std::pair<int, json> sendHttpRequest(HttpClient& client, const std::string& method, 
                                    const std::string& path, const json& data = json::object()) {
    // 构建请求体
    std::string message_body = data.dump();
    
    // 构建HTTP请求
    std::string http_request = 
        method + " " + path + " HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "User-Agent: C++HttpClient/1.0\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(message_body.size()) + "\r\n"
        "\r\n" + message_body;

    // 发送请求并获取响应
    std::string response_str = client.sendRequest(http_request);
    
    // 解析响应
    return parseHttpResponse(response_str);
}

int main() {
    try {
        HttpClient client("192.168.38.121", 8080);
        
        if (!client.connect()) {
            std::cerr << "Connection failed" << std::endl;
            return 1;
        }

        int choice;
        int user_id = 0; // 存储用户ID，初始化为0
        
        while (true) {
            std::cout << "输入1/2/3---注册/登录/退出: ";
            std::cin >> choice;
            std::cin.ignore(); // 清除输入缓冲区

            if (choice == 3) {
                std::cout << "退出程序" << std::endl;
                break;
            }

            json request_data;
            std::string path;
            
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
                std::cerr << "无效的选择，请重新输入" << std::endl;
                continue;
            }

            // 发送请求
            auto [status_code, response_json] = sendHttpRequest(client, "POST", path, request_data);
            
            std::cout << "响应状态码: " << status_code << std::endl;
            std::cout << "响应内容: " << response_json.dump(4) << std::endl;
            
            // 检查登录/注册是否成功
            if (status_code == 200) {
                if (choice == 1) {
                    std::cout << "注册成功" << std::endl;
                } else {
                    std::cout << "登录成功" << std::endl;
                }
                
                // 获取用户ID
                if (response_json.contains("user_id")) {
                    if (response_json["user_id"].is_number()) {
                        user_id = response_json["user_id"].get<int>();
                    } else if (response_json["user_id"].is_string()) {
                        user_id = std::stoi(response_json["user_id"].get<std::string>());
                    }
                    std::cout << "已获取用户ID: " << user_id << std::endl;
                }
                
                // 进入登录后的操作循环
                while (true) {
                    std::cout << "输入1/2/3---发帖/退出登录/退出程序: ";
                    std::cin >> choice;
                    std::cin.ignore();
                    
                    if (choice == 2) {
                        std::cout << "退出登录" << std::endl;
                        user_id = 0;
                        break;
                    } else if (choice == 3) {
                        std::cout << "退出程序" << std::endl;
                        return 0;
                    } else if (choice != 1) {
                        std::cerr << "无效的选择，请重新输入" << std::endl;
                        continue;
                    }
                    
                    // 发帖
                    std::string content;
                    std::cout << "请输入帖子内容: ";
                    std::getline(std::cin, content);
                    
                    json post_data;
                    post_data["content"] = content;
                    post_data["user_id"] = user_id; // 添加用户ID到消息体
                    
                    std::cout << "发送的消息体: " << post_data.dump() << std::endl;
                    
                    // 发送请求
                    auto [post_status, post_response] = sendHttpRequest(client, "POST", "/api/post/publish", post_data);
                    
                    std::cout << "响应状态码: " << post_status << std::endl;
                    std::cout << "响应内容: " << post_response.dump(4) << std::endl;
                    
                    if (post_status == 200 && post_response.contains("post_id")) {
                        std::cout << "发帖成功，帖子ID: ";
                        if (post_response["post_id"].is_number()) {
                            std::cout << post_response["post_id"].get<int>() << std::endl;
                        } else if (post_response["post_id"].is_string()) {
                            std::cout << post_response["post_id"].get<std::string>() << std::endl;
                        }
                    }
                }
            } else {
                std::cout << "操作失败" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}