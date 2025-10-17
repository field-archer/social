#pragma once
#include"PostDAO.h"
#include"UserDAO.h"

class PostService
{
private:
    PostDAO postDAO_;               //DAO数据操作层

    bool UserExist(int _userId);                                        //检查用户存在
    std::vector<Post> HandleCheckPostsByUserId(int _userId);           //根据userId查看某人帖子 
public:
    PostService(const PostDAO& _postDAO);   //构造函数
    PostService()=default;                  //构造函数
    ~PostService()=default;                 //析构函数
    
    //发表贴子，发表后Post所存贴子内容丢失，返回贴子id/-1（未知错误）
    int HandlePublishPost(std::unique_ptr<std::string> _content,int _userId);               
    
    bool HandleDeletePost(int _postId);                //删除贴子
    
    std::vector<Post> HandleCheckMyPosts(int _userId);                //查看用户自己的贴子
};