#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdint>

class TCPClient {
private:
    int sockfd;
    struct sockaddr_in server_addr;

    // 发送数据（先发送4字节长度头，再发送实际数据）
    bool sendData(const std::string& data) {
        // 准备长度头（网络字节序）
        uint32_t len = htonl(static_cast<uint32_t>(data.size()));
        
        // 先发送长度头
        if (send(sockfd, &len, sizeof(len), 0) != sizeof(len)) {
            std::cerr << "Failed to send length header" << std::endl;
            return false;
        }
        
        // 再发送实际数据
        if (send(sockfd, data.c_str(), data.size(), 0) != static_cast<ssize_t>(data.size())) {
            std::cerr << "Failed to send data" << std::endl;
            return false;
        }
        
        return true;
    }

    // 接收数据（先接收4字节长度头，再接收实际数据）
    bool recvData(std::string& data) {
        uint32_t len_net;
        
        // 先接收长度头
        if (recv(sockfd, &len_net, sizeof(len_net), 0) != sizeof(len_net)) {
            std::cerr << "Failed to receive length header" << std::endl;
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
                std::cerr << "Failed to receive data" << std::endl;
                delete[] buffer;
                return false;
            }
            received += bytes;
        }
        
        buffer[len] = '\0';
        data.assign(buffer, len);
        delete[] buffer;
        
        return true;
    }

public:
    TCPClient(const std::string& ip, uint16_t port) {
        // 创建socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        
        // 设置服务器地址
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
            close(sockfd);
            throw std::runtime_error("Invalid IP address");
        }
    }
    
    ~TCPClient() {
        if (sockfd >= 0) {
            close(sockfd);
        }
    }
    
    // 连接到服务器
    bool connectToServer() {
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Connection failed" << std::endl;
            return false;
        }
        std::cout << "Connected to server" << std::endl;
        return true;
    }
    
    // 执行一次完整的发送-接收操作
    bool communicate(const std::string& message) {
        if (!sendData(message)) {
            return false;
        }
        
        std::string response;
        if (!recvData(response)) {
            return false;
        }
        
        std::cout << "Received from server: " << response << std::endl;
        return true;
    }
};

int main() {
    try {
        TCPClient client("192.168.232.135", 8080);
        
        if (!client.connectToServer()) {
            return 1;
        }
        
        std::string input;
        while (true) {
            std::cout << "Enter message to send (type 'exit' to quit): ";
            std::getline(std::cin, input);
            
            if (input == "exit") {
                break;
            }
            
            if (!client.communicate(input)) {
                std::cerr << "Communication failed" << std::endl;
                break;
            }
        }
        
        std::cout << "Client exiting..." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}