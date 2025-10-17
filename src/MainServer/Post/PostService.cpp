#include"PostService.h"

//构造函数
PostService:: PostService(const PostDAO& _postDAO):postDAO_(_postDAO)
{

}  

//发表贴子，发表后Post所存贴子内容丢失，返回贴子id/-1（未知错误）
int PostService::HandlePublishPost(std::unique_ptr<std::string> _content,int _userId)
{
    //发表贴子
    try
    {
        //检查userId对应用户是否存在
        if(!UserExist(_userId))throw std::runtime_error("用户不存在");

        //发表贴子
        Post post (std::move(_content),_userId);
        int postId=postDAO_.PublishPost(std::move(post));
        return postId;
    }
    catch(const std::exception& e)
    {
        //直接throw，无需补充
        throw std::runtime_error(e.what());
    }
}
//删除贴子
bool PostService::HandleDeletePost(int _postId)
{
    try
    {
        return postDAO_.DeletePost(_postId);
    }
    catch(const std::exception& e)
    {
        //直接throw，无需补充
        throw std::runtime_error(e.what());
    }
    
}
//根据userId查看某人帖子 
std::vector<Post> PostService::HandleCheckPostsByUserId(int _userId)
{
    try
    {
        //检查用户是否存在
        if(!UserExist(_userId))throw std::runtime_error("用户不存在");
        return postDAO_.SelectPostsByUserId(_userId);
    }
    catch(const std::exception& e)
    {//出错抛出，无需补充
        throw std::runtime_error(e.what());
    }
    
}
//查看用户自己的贴子
std::vector<Post> PostService::HandleCheckMyPosts(int _userId)
{
    try
    {
        return HandleCheckPostsByUserId(_userId);
    }
    catch(const std::exception& e)
    {//出错抛出，无需补充
        throw std::runtime_error(e.what());
    }
    
}
//检查用户存在
bool PostService::UserExist(int _userId)
{
    try
    {
        UserDAO userDAO(postDAO_.GetDBPool());//获取UserDAO
        return userDAO.exist(_userId);
    }
    catch(const std::exception& e)
    {//出错抛出，无需补充
        throw std::runtime_error(e.what());
    }
    
}