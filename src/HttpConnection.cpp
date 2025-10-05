#include"HttpConnection.h"
HttpConnection::HttpConnection(Connection* _connection):connection_(_connection)
{

}
HttpConnection::~HttpConnection()
{
    
}
void HttpConnection::send(HttpResponse* _reponse)
{
    std::string _message="HTTP/1.1 "+std::to_string(_reponse->GetStatusCode_())+" "+_reponse->GetstatusMessage()+"\r\n"
                        +_reponse->GetHeader()+"\r\n"+_reponse->GetBody();

    uint32_t len = htonl(static_cast<uint32_t>(_message.size()));
    // _message=std::to_string(len)+_message;
    printf("要发送的消息:%s\n",_message.c_str());
    connection_->send(_message);
}
 