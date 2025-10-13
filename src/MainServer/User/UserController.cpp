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
        // json j=json::parse(std::move(_context->GetRequestBody()));
        json j=json::parse(std::move(_context->GetRequest().GetBody()));
        std::string name=j["name"].get<std::string>();
        std::string email=j["email"].get<std::string>();
        std::string passwd=j["passwd"].get<std::string>();

        if(userService_->HandleSignUp(name,email,passwd))
        {//注册成功
            _context->GetResponse().SetStatusCode(201);
            _context->GetResponse().SetSatusMessage("OK");
            _context->GetResponse().SetBody(R"({"message": "用户注册成功!"})");
            _context->SetUsefulHead();
            _context->GetConnection().send(_context->GetResponse());
            return true;
        }else 
        {//注册失败
            _context->GetResponse().SetStatusCode(500);
            _context->GetResponse().SetSatusMessage("error");
            _context->GetResponse().SetBody(R"({"message": "用户注册失败!"})");
            _context->SetUsefulHead();
            _context->GetConnection().send(_context->GetResponse());
            return false;
        }
    }
    catch(const std::exception& e)
    {//解析失败
        _context->GetResponse().SetStatusCode(400);
        _context->GetResponse().SetSatusMessage("error");
        _context->GetResponse().SetBody(std::string(R"({"message": ")")+e.what()+std::string(R"("})"));
        _context->SetUsefulHead();
        _context->GetConnection().send(_context->GetResponse());
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

        if(userService_->HandleLogIn(email,passwd))
        {//登录成功
            _context->GetResponse().SetStatusCode(201);
            _context->GetResponse().SetSatusMessage("OK");
            _context->GetResponse().SetBody(R"({"message": "用户登录成功!"})");
            _context->SetUsefulHead();
            _context->GetConnection().send(_context->GetResponse());
            return true;
        }else 
        {//登录失败
            _context->GetResponse().SetStatusCode(500);
            _context->GetResponse().SetSatusMessage("error");
            _context->GetResponse().SetBody(R"({"message": "用户登录失败!"})");
            _context->SetUsefulHead();
            _context->GetConnection().send(_context->GetResponse());
            return false;
        }
    }
    catch(const std::exception& e)
    {//解析失败
        _context->GetResponse().SetStatusCode(400);
        _context->GetResponse().SetSatusMessage("error");
        _context->GetResponse().SetBody(std::string(R"({"message": ")")+e.what()+std::string(R"("})"));
        _context->SetUsefulHead();
        _context->GetConnection().send(_context->GetResponse());
        return false;
    }
}