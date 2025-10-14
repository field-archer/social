include_directories(${CMAKE_SOURCE_DIR}/include/Http
                    ${CMAKE_SOURCE_DIR}/include/MainServer
                    ${CMAKE_SOURCE_DIR}/include/MainServer/User
                    ${CMAKE_SOURCE_DIR}/include/MainServer/Post
                    ${CMAKE_SOURCE_DIR}/include/MYSQL
                    ${CMAKE_SOURCE_DIR}/include/Tcp
                    ${CMAKE_SOURCE_DIR}/include/Tool)

aux_source_directory(Http Http)
aux_source_directory(MainServer MainServer)
aux_source_directory(MainServer/User User)
aux_source_directory(MainServer/Post Post)
aux_source_directory(MYSQL MYSQL)
aux_source_directory(Tcp Tcp)
aux_source_directory(Tool Tool)
set(server main.cpp ${Http} ${MainServer} ${MYSQL} ${Tcp} ${Tool} ${User} ${Post})

set(client Client/httpClient.cpp)

find_package(mysql-concpp REQUIRED)#MYSQL的xdevapi
find_package(nlohmann_json 3.11.3 REQUIRED)

add_executable(client ${client})
add_executable(server ${server})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(server PRIVATE mysql::concpp-xdevapi)#MYSQL的xdevapi
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(client PRIVATE nlohmann_json::nlohmann_json)


