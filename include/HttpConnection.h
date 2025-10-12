#pragma once
#include"HttpResponse.h"
#include"Connectio.h"
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