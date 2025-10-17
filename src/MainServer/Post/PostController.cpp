#include"PostController.h"

//构造函数
PostController:: PostController(const PostService& _postService):postService_(_postService)
{

}

//发表贴子
bool PostController::HandlePublishPost(std::unique_ptr<HttpContext> _context)
{
    try
    {
        //解析json数据
        json j=json::parse(_context->GetRequest().GetBody());
        //获取context和userId
        std::unique_ptr<std::string> content=std::make_unique<std::string>(std::move(j["content"].get<std::string>()));
        int userId=j["user_id"].get<int>();
        //发表贴子
        int postId=postService_.HandlePublishPost(std::move(content),userId);
        if(postId!=-1)
        {//发表成功
            std::cerr<<"发帖成功\n";
            //构建http响应并发送
            //json数据构建响应体
            json j=
            {
                {"post_id",postId},
                {"message","发帖成功"}
            };
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(200,"OK");
            _context->Send();
            return true;
        }else 
        {//发表失败
            std::cerr<<"发帖失败\n";

            //构建http响应并发送
            //json数据构建响应体
            json j=
            {
                {"message","发帖失败，请稍后再试"}
            };
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(500,"error");
            _context->Send();
            return false;
        }
    }
    catch(const std::exception& e)
    {//出错
        std::cerr<<"发帖失败且出错\n";

        //构建http响应并发送
        //json数据构建响应体
        json j=
        {
            {"error",e.what()}
        };
        _context->SetResponseBody(std::move(j.dump()));
        _context->SetReSponseStatusANDUsefulHead(400,"error");
        _context->Send();

        return false;
    }
}
//删除贴子
bool PostController::HandleDeletePost(std::unique_ptr<HttpContext> _context)
{
    try
    {
        //解析json数据
        json j=json::parse(_context->GetRequest().GetBody());
        //获取post_id
        int postId=j["post_id"].get<int>();
        //删除贴子
        if(postService_.HandleDeletePost(postId))
        {//发表成功

            //构建http响应并发送
            //json数据构建响应体
            json j=
            {
                {"message","删除贴子成功"}
            };
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(200,"OK");
            _context->Send();

            return true;
        }else 
        {//发表失败
            std::cerr<<"删帖失败\n";
            //构建http响应并发送
            //json数据构建响应体
            json j=
            {
                {"message","删除贴子失败"}
            };
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(500,"error");
            _context->Send();

            return false;
        }
    }
    catch(const std::exception& e)
    {//出错
        std::cerr<<"删帖失败且出错\n";
        //构建http响应并发送
        //json数据构建响应体
        json j=
        {
            {"error",e.what()}
        };
        _context->SetResponseBody(std::move(j.dump()));
        _context->SetReSponseStatusANDUsefulHead(400,"error");
        _context->Send();

        return false;
    }
}
//查看用户自己的贴子
bool PostController::HandleMyPosts(std::unique_ptr<HttpContext> _context)
{
    try
    {
        //获取用户id
        json j=json::parse(_context->GetRequest().GetBody());
        int userId=j["user_id"].get<int>();
        //获取贴子
        std::vector<Post> posts=postService_.HandleCheckMyPosts(userId);
        //根据情况发送http响应
        if(posts.size()!=0)
        {//查看贴子成功
            //构建http响应并发送
            //json数据构建响应体
            json jsonPosts=json::array();
            for(int i=0;i<posts.size();i++)
            {
                jsonPosts.push_back(std::move(posts[i].ToJson()));
            }
            json j=
            {
                {"message","查看贴子成功"},
                {"posts",jsonPosts}
            };
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(200,"OK");
            _context->Send();
            return true;
        }else 
        {//查看帖子失败
            //构建http响应并发送
            //json数据构建响应体
            json j=
            {
                {"message","查看贴子失败"},
            };
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(500,"error");
            _context->Send();
            return false;
        }
    }
    catch(const std::exception& e)
    {//查看贴子出错
        //构建http响应并发送
        //json数据构建响应体
        json j=
        {
            {"error",e.what()},
        };
        _context->SetResponseBody(std::move(j.dump()));
        _context->SetReSponseStatusANDUsefulHead(400,"error");
        _context->Send();
        return false;
    }
    
}   
