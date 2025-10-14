#pragma once
#include"PostDAO.h"
#include"UserDAO.h"

class PostService
{
private:
    PostDAO postDAO_;               //DAO数据操作层
public:
    PostService(const PostDAO& _postDAO);   //构造函数
    PostService()=default;                  //构造函数
    ~PostService()=default;                 //析构函数

    bool HandlePublishPost(std::unique_ptr<std::string> _content,int _userId);               //发表贴子
    bool HandleDeletePost(int _postId);                //删除贴子
};