#include"HttpClient.h"
int main()
{
    HttpClient httpClient("192.168.38.121",8080);
    int op=-1;
    std::cout<<"请输入操作(1:注册 2:登录 3:退出)\n";
    std::cin>>op;
    if(op==1)
    {
        
    }else if(op==2)
    {

    }else if(op==3)
    {

    }
    return 0;
}