#include"Post.h"
//构造函数，post_移动
Post::Post(std::unique_ptr<std::string> _content,int _userId):postId_(-1),content_(std::move(_content)),userId_(_userId)
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