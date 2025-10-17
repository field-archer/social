#pragma once
#include<string>
#include<nlohmann/json.hpp>
#include<memory>

using json=nlohmann::json;

class Post
{
private:
    int postId_;            //贴子id
    std::unique_ptr<std::string> content_;      //贴子内容
    int userId_;            //用户id
    std::string createTime_;         //贴子创建时间，格式：YYYY-MM-DD-HH-MM-SS
public:

    //删除拷贝赋值函数和拷贝构造函数
    Post& operator=(const Post& _post)=delete;
    Post(const Post& _post)=delete;
    //定义移动赋值函数和移动构造函数
    Post& operator=(Post&& _post) noexcept;
    Post(Post&& _post) noexcept;

    Post(std::unique_ptr<std::string> _content,int _userId);   //构造函数，post_移动
    Post(int _postId,std::unique_ptr<std::string> _content,int _userId,std::string _createTime);   //构造函数，post_移动
    ~Post()=default;

    int GetPostId();                                        //返回贴子id
    std::unique_ptr<std::string> GetContent_unique();       //返回贴子内容的unique指针，慎用
    std::string GetContent();                               //返回贴子内容的副本
    int GetUserId();                                        //返回发帖者id
    std::string GetCreateTime() const;                            //返回发帖时间

    void SetPostId(int _postId);                                       //设置贴子id

    json ToJson();                                          //转换为json数据
};