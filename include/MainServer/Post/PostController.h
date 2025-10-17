#pragma once
#include"PostService.h"
#include"HttpContext.h"
#include<nlohmann/json.hpp>
using json=nlohmann::json;

class PostController
{
private:
    PostService postService_;
public:
    PostController(const PostService& _postService);//构造函数
    PostController()=default;                       //构造函数
    ~PostController()=default;                      //析构函数

    bool HandlePublishPost(std::unique_ptr<HttpContext> _context);      //发表贴子
    bool HandleDeletePost(std::unique_ptr<HttpContext> _context);                                 //删除贴子

    bool HandleMyPosts(std::unique_ptr<HttpContext> _context);           //查看用户自己的贴子
};