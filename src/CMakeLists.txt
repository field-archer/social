include_directories(../include)

set(source main.cpp TcpServer.cpp Acceptor.cpp EventLoop.cpp Epoll.cpp Channel.cpp Socket.cpp InetAddr.cpp Connection.cpp Http.cpp  Buffer.cpp)    

add_executable(server ${source})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)