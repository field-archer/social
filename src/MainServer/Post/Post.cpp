#include"Post.h"
//移动赋值函数
Post& Post::operator=(Post&& _post) noexcept
{
    postId_=std::move(_post.postId_);
    content_=std::move(_post.content_);
    userId_=std::move(_post.userId_);
    createTime_=std::move(_post.createTime_);
    return *this;
}
//移动构造函数
Post::Post(Post&& _post) noexcept:
    postId_(std::move(_post.postId_)),content_(std::move(_post.content_)),
    userId_(std::move(_post.userId_)),createTime_(std::move(_post.createTime_))
{

}
//构造函数，post_移动
Post::Post(std::unique_ptr<std::string> _content,int _userId):
    postId_(-1),content_(std::move(_content)),userId_(_userId),createTime_("")
{

}
//构造函数，post_移动
Post::Post(int _userId,std::unique_ptr<std::string> _content,int _postId,std::string _createTime):
    postId_(_postId),content_(std::move(_content)),userId_(_userId),createTime_(std::move(_createTime))
{

}   
//返回贴子id
int Post::GetPostId()
{
    return postId_;
}   
//返回贴子内容的unique指针，慎用
std::unique_ptr<std::string> Post::GetContent_unique()
{
    return std::move(content_);
}
//返回贴子内容的副本
std::string Post::GetContent()
{
    return *content_;
}
//返回发帖者id
int Post::GetUserId()
{
    return userId_;
}

//设置贴子id
void Post::SetPostId(int _postId)
{
    postId_=_postId;
}   
//转换为json数据
json Post::ToJson()
{
    json j=
    {
        {"post_id",postId_},
        {"content",*content_},
        {"user_id",userId_},
        {"create_time",createTime_}
    };
    return j;
}