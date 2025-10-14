#pragma once
#include"Post.h"
#include"MYSQLConnectionPool.h"


class PostDAO
{
private:
    spDBPool dbPool_;       //mysql连接池
public:
    PostDAO(spDBPool _dbPool);      //构造函数
    PostDAO()=default;                      //构造函数
    ~PostDAO()=default;

    bool PublishPost(std::unique_ptr<Post> _post);             //发表贴子，发表后Post所存贴子内容丢失
    bool DeletePost(int _postId);              //删除贴子
    spDBPool GetDBPool();                                       //返回连接池
};