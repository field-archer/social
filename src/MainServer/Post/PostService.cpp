#include"PostService.h"

//构造函数
PostService:: PostService(const PostDAO& _postDAO):postDAO_(_postDAO)
{

}  

//发表贴子
int PostService::HandlePublishPost(std::unique_ptr<std::string> _content,int _userId)
{
    //检查userId对应用户是否存在
    UserDAO userDAO(postDAO_.GetDBPool());//获取UserDAO
    if(!userDAO.exist(_userId))return false;
    //发表贴子
    try
    {
        std::unique_ptr<Post> post (new Post(std::move(_content),_userId));
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