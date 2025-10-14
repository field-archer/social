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
        if(postService_.HandlePublishPost(std::move(content),userId))
        {//发表成功
            std::cerr<<"发帖成功\n";
            _context->SetUsefulHead();
            _context->GetResponse().SetStatusCode(200);
            _context->GetResponse().SetSatusMessage("OK");
            std::string body=R"({"message":"发表贴子成功!"})";
            _context->GetResponse().SetBody(body);
            _context->GetConnection().send(_context->GetResponse());
            return true;
        }else 
        {//发表失败
            std::cerr<<"发帖失败\n";
            _context->SetUsefulHead();
            _context->GetResponse().SetStatusCode(500);
            _context->GetResponse().SetSatusMessage("error");
            std::string body=R"({"message":"发表贴子失败!"})";
            _context->GetResponse().SetBody(body);
            _context->GetConnection().send(_context->GetResponse());
            return false;
        }
    }
    catch(const std::exception& e)
    {//出错
        std::cerr<<"发帖失败且出错\n";
        _context->SetUsefulHead();
        _context->GetResponse().SetStatusCode(400);
        _context->GetResponse().SetSatusMessage("error");
        std::string body=R"({"error":"发表贴子失败,)"+std::string(e.what())+"})";
        _context->GetResponse().SetBody(body);
        _context->GetConnection().send(_context->GetResponse());
        return false;
        // throw std::runtime_error("发表贴子失败:"+std::string(e.what()));
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
            std::cerr<<"删帖成功\n";
            _context->SetUsefulHead();
            _context->GetResponse().SetStatusCode(200);
            _context->GetResponse().SetSatusMessage("OK");
            std::string body=R"({"message":"删除贴子成功!"})";
            _context->GetResponse().SetBody(body);
            _context->GetConnection().send(_context->GetResponse());
            return true;
        }else 
        {//发表失败
            std::cerr<<"删帖失败\n";
            _context->SetUsefulHead();
            _context->GetResponse().SetStatusCode(500);
            _context->GetResponse().SetSatusMessage("error");
            std::string body=R"({"message":"删除贴子失败!"})";
            _context->GetResponse().SetBody(body);
            _context->GetConnection().send(_context->GetResponse());
            return false;
        }
    }
    catch(const std::exception& e)
    {//出错
        std::cerr<<"删帖失败且出错\n";
        _context->SetUsefulHead();
        _context->GetResponse().SetStatusCode(400);
        _context->GetResponse().SetSatusMessage("error");
        std::string body=R"({"error":"删除贴子失败,)"+std::string(e.what())+"})";
        _context->GetResponse().SetBody(body);
        _context->GetConnection().send(_context->GetResponse());
        return false;
        // throw std::runtime_error("发表贴子失败:"+std::string(e.what()));
    }
}
