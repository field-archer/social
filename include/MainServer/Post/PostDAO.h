#pragma once
#include<vector>
#include"Post.h"
#include"MYSQLConnectionPool.h"



class PostDAO
{
private:
    spDBPool dbPool_;       //mysql连接池
public:
    PostDAO(spDBPool _dbPool);      //构造函数
    PostDAO(const PostDAO& _postDAO);       //拷贝构造函数
    PostDAO()=default;                      //构造函数
    ~PostDAO()=default;

    int PublishPost(Post _post);             //发表贴子，发表后Post所存贴子内容丢失,返回贴子id/-1
    bool DeletePost(int _postId);              //删除贴子
    spDBPool GetDBPool();                                       //返回连接池

    std::vector<Post> SelectPostsByUserId(int _userId);                                  //查看某用户所有贴子
};