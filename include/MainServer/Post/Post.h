#pragma once
#include<string>
#include<memory>
class Post
{
private:
    int postId_;            //贴子id
    std::unique_ptr<std::string> content_;      //贴子内容
    int userId_;            //用户id
public:
    Post(std::unique_ptr<std::string> _content,int _userId);   //构造函数，post_移动
    ~Post()=default;

    int GetPostId();                                        //返回贴子id
    std::unique_ptr<std::string> GetContent_unique();       //返回贴子内容的unique指针，慎用
    std::string GetContent();                               //返回贴子内容的副本
    int GetUserId();                                        //返回发帖者id

    void SetPostId(int _postId);                                       //设置贴子id
};