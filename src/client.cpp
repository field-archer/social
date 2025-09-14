#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdint>
#include <functional>

// 全局变量
int sockfd = -1;
bool is_connected = false;

// 发送数据函数
bool send_data(const std::string& data) {
    if (sockfd < 0 || !is_connected) {
        std::cerr << "错误：未连接到服务器" << std::endl;
        return false;
    }
    
    // 准备长度头（网络字节序）
    uint32_t len = htonl(static_cast<uint32_t>(data.size()));
    
    // 先发送长度头
    if (send(sockfd, &len, sizeof(len), 0) != sizeof(len)) {
        std::cerr << "错误：发送长度头失败" << std::endl;
        return false;
    }
    
    // 再发送实际数据
    if (send(sockfd, data.c_str(), data.size(), 0) != static_cast<ssize_t>(data.size())) {
        std::cerr << "错误：发送数据失败" << std::endl;
        return false;
    }
    
    std::cout << "已发送 " << data.size() << " 字节数据" << std::endl;
    return true;
}

// 接收数据函数
bool recv_data(std::string& data) {
    if (sockfd < 0 || !is_connected) {
        std::cerr << "错误：未连接到服务器" << std::endl;
        return false;
    }
    
    uint32_t len_net;
    
    // 先接收长度头
    if (recv(sockfd, &len_net, sizeof(len_net), 0) != sizeof(len_net)) {
        std::cerr << "错误：接收长度头失败" << std::endl;
        return false;
    }
    
    // 转换为主机字节序
    uint32_t len = ntohl(len_net);
    
    if (len == 0) {
        data.clear();
        return true;
    }
    
    // 分配缓冲区并接收数据
    char* buffer = new char[len + 1];
    ssize_t received = 0;
    
    while (received < static_cast<ssize_t>(len)) {
        ssize_t bytes = recv(sockfd, buffer + received, len - received, 0);
        if (bytes <= 0) {
            std::cerr << "错误：接收数据失败" << std::endl;
            delete[] buffer;
            return false;
        }
        received += bytes;
    }
    
    buffer[len] = '\0';
    data.assign(buffer, len);
    delete[] buffer;
    
    std::cout << "已接收 " << len << " 字节数据" << std::endl;
    return true;
}

// 连接到服务器函数
bool connect_to_server(const std::string& ip, uint16_t port) {
    // 创建socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "错误：创建socket失败" << std::endl;
        return false;
    }
    
    // 设置服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "错误：无效的IP地址" << std::endl;
        close(sockfd);
        sockfd = -1;
        return false;
    }
    
    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "错误：连接服务器失败" << std::endl;
        close(sockfd);
        sockfd = -1;
        return false;
    }
    
    is_connected = true;
    std::cout << "已连接到服务器 " << ip << ":" << port << std::endl;
    return true;
}

// 断开连接函数
void disconnect() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
    is_connected = false;
    std::cout << "已断开连接" << std::endl;
}

// 清理函数
void cleanup() {
    disconnect();
}

// 主函数
int main() {
    const std::string server_ip = "192.168.109.42";
    const uint16_t server_port = 8080;
    
    // 注册清理函数
    atexit(cleanup);
    
    // 连接到服务器
    if (!connect_to_server(server_ip, server_port)) {
        return 1;
    }
    
    std::string input;
    while (true) {
        std::cout << "请输入要发送的消息（输入'exit'退出）: ";
        std::getline(std::cin, input);
        
        if (input == "exit") {
            break;
        }
        
        // 发送数据
        if (!send_data(input)) {
            std::cerr << "发送失败，尝试重新连接..." << std::endl;
            disconnect();
            if (!connect_to_server(server_ip, server_port)) {
                std::cerr << "重新连接失败，程序退出" << std::endl;
                break;
            }
            continue;
        }
        
        // 接收响应
        std::string response;
        if (!recv_data(response)) {
            printf("接收到:%s\n",response.c_str());
            std::cerr << "接收失败，尝试重新连接..." << std::endl;
            disconnect();
            if (!connect_to_server(server_ip, server_port)) {
                std::cerr << "重新连接失败，程序退出" << std::endl;
                break;
            }
            continue;
        }
        
        std::cout << "服务器响应: " << response << std::endl;
    }
    
    return 0;
}