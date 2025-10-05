#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

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
        // char len_buf[4];
        int bytes_read = recv(sock, &response_len_net, 4, 0);
        if (bytes_read != 4) {
            return "Failed to read length prefix";
        }
        // memcpy(&response_len_net, len_buf, 4);
        uint32_t response_len = ntohl(response_len_net);

        // 接收响应主体
        printf("接收到的长度为%d\n",response_len);
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
        HttpClient client("127.0.0.1", 8080);
        
        if (!client.connect()) {
            std::cerr << "Connection failed" << std::endl;
            return 1;
        }

        // 构造HTTP请求
        std::string http_request = 
            "GET / HTTP/1.1\r\n"
            "Host: localhost:8080\r\n"
            "Connection: close\r\n"
            "User-Agent: C++HttpClient/1.0\r\n"
            "\r\n";

        // 发送请求并获取响应
        std::string response = client.sendRequest(http_request);
        
        std::cout << "Server response:\n" << response << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}