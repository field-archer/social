include_directories(../include)

set(source main.cpp TcpServer.cpp Acceptor.cpp EventLoop.cpp Epoll.cpp 
            Channel.cpp Socket.cpp InetAddr.cpp Connection.cpp Buffer.cpp 
            HttpServer.cpp HttpConnection.cpp HttpRequest.cpp HttpResponse.cpp
            ThreadPool.cpp TimeStamp.cpp MYSQLConnectionPool.cpp HttpContext.cpp MainServer.cpp)    
set(source2 client.cpp)

find_package(mysql-concpp REQUIRED)#MYSQL的xdevapi
find_package(nlohmann_json 3.11.3 REQUIRED)

add_executable(client ${source2})
add_executable(server ${source})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(server PRIVATE mysql::concpp-xdevapi)#MYSQL的xdevapi
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(client PRIVATE nlohmann_json::nlohmann_json)


