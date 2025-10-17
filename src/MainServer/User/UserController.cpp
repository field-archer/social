#include"UserController.h"

//移动构造函数(UserController)
UserController::UserController(UserController&& _userController) noexcept:
                userService_(std::move(_userController.userService_))
{

}
//移动构造函数(UserService指针)
UserController::UserController(std::unique_ptr<UserService>&& _userService) noexcept:
                userService_(std::move(_userService))
{

}
//移动赋值(UserService指针)
UserController& UserController::operator=(std::unique_ptr<UserService>&& _userService)noexcept
{
    userService_=std::move(_userService);
    return *this;
}
//移动赋值(UserController)
UserController& UserController::operator=(UserController&& _userController)noexcept
{
    userService_=std::move(_userController.userService_);
    return *this;
}
//注册函数
bool UserController::HandleSignUp(std::unique_ptr<HttpContext> _context)
{
    try
    {
        //从请求体中解析出json数据
        json j=json::parse(std::move(_context->GetRequest().GetBody()));
        std::string name=j["name"].get<std::string>();
        std::string email=j["email"].get<std::string>();
        std::string passwd=j["passwd"].get<std::string>();

        int userId=userService_->HandleSignUp(name,email,passwd);
        if(userId!=0)
        {//注册成功
            json j;
            j["message"]="用户注册成功";
            j["user_id"]=userId;
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(200,"OK");
            _context->Send();
            return true;
        }else 
        {//注册失败
            json j;
            j["message"]="用户注册失败";
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(500,"error");
            _context->Send();
            return false;
        }
    }
    catch(const std::exception& e)
    {//解析失败
        json j;
        j["message"]="用户注册失败";
        _context->SetResponseBody(std::move(j.dump()));
        _context->SetReSponseStatusANDUsefulHead(400,"error");
        _context->Send();
        return false;
    }
}
//登录函数
bool UserController::HandleLogIn(std::unique_ptr<HttpContext> _context)
{
    try
    {
        //从请求体中解析出json数据
        // json j=json::parse(std::move(_context->GetRequestBody()));
        json j=json::parse(std::move(_context->GetRequest().GetBody()));
        std::string email=j["email"].get<std::string>();
        std::string passwd=j["passwd"].get<std::string>();
        
        int userId=userService_->HandleLogIn(email,passwd);
        if(userId!=0)
        {//登录成功
            json j;
            j["message"]="用户登录成功";
            j["user_id"]=userId;
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(200,"OK");
            _context->Send();
            return true;
        }else 
        {//登录失败
            json j;
            j["message"]="用户登录失败";
            _context->SetResponseBody(std::move(j.dump()));
            _context->SetReSponseStatusANDUsefulHead(500,"error");
            _context->Send();
            return false;
        }
    }
    catch(const std::exception& e)
    {//解析失败
        json j;
        j["error"]=e.what();
        _context->SetResponseBody(std::move(j.dump()));
        _context->SetReSponseStatusANDUsefulHead(400,"error");
        _context->Send();
        return false;
    }
}