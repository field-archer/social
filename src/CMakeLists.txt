include_directories(../include)

set(source main.cpp TcpServer.cpp Acceptor.cpp EventLoop.cpp Epoll.cpp Channel.cpp Socket.cpp InetAddr.cpp Connection.cpp Buffer.cpp EchoTcpServer.cpp)    
set(source2 client.cpp)

add_executable(client ${source2})
add_executable(server ${source})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)