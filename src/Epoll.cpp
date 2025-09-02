#include"Epoll.h"
Epoll::Epoll()                        //构造函数
{
    epfd=epoll_create(1024);//历史遗留1024
    memset(evs,0,sizeof(evs));
}
Epoll::~Epoll()                       //析构函数
{
    
}
void Epoll::UpdateChannel(Channel *_channel)           //更新Channel
{
    epoll_event tmpEv;
    tmpEv.data.ptr=_channel;
    tmpEv.events=_channel->Tevent();
    if(_channel->inEpoll())//已在epoll中，修改
    {
        if(epoll_ctl(epfd,EPOLL_CTL_MOD,_channel->fd(),&tmpEv))
        {
            printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
            perror(":");
            exit(-1);
        }
    }else                 //不在epoll中，添加
    {
        if(epoll_ctl(epfd,EPOLL_CTL_ADD,_channel->fd(),&tmpEv))
        {
            printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
            perror(":");
            exit(-1);
        }
        _channel->SetInEpoll();//设置inEpoll
        printf("已将一个channel加入epoll中\n");
    }
}
std::vector<Channel*>Epoll::loop(int time)              //返回有事件发生的Channel*数组
{
    std::vector<Channel*>re(0);
    printf("开始epoll_wait\n");
    int num=epoll_wait(epfd,evs,maxsize,time);
    printf("有%d个事件\n",num);
    if(num==0)             //-1超时
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(",epoll_wait超时:");
        exit(-1);
    }else if(num==-1)        //0出错
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(",epoll_wait出错:");
        exit(-1);
    }

    for(int i=0;i<num;i++)  //将evs的数据转移到Channel*数组中
    {
        Channel *tmpChannel=(Channel*)evs[i].data.ptr;
        tmpChannel->SetRevent(evs[i].events);
        re.push_back(tmpChannel);
    }
    return re;
}