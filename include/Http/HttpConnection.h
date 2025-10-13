#pragma once
#include"HttpResponse.h"
#include"Connection.h"
class Connection;
class HttpResponse;

class HttpConnection
{
private:
    Connection* connection_;
public:
    
    HttpConnection(Connection* _connection);
    ~HttpConnection();

    void send(HttpResponse& reponse);
};