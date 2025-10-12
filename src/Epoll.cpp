#include"Epoll.h"
Epoll::Epoll()                                          //构造函数
{
    epfd=epoll_create(1024);//历史遗留1024
    memset(evs,0,sizeof(evs));
}
Epoll::~Epoll()                                         //析构函数
{
    //无需析构
}
void Epoll::UpdateChannel(Channel *_channel)            //更新Channel(mod或add)
{
    epoll_event tmpEv;
    memset(&tmpEv, 0, sizeof(tmpEv));
    tmpEv.data.ptr=_channel;
    tmpEv.events=_channel->Tevent();
    if(_channel->inEpoll())//已在epoll中，修改
    {
        if(epoll_ctl(epfd,EPOLL_CTL_MOD,_channel->fd(),&tmpEv))
        {
            printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
            perror(":");
            exit(-1);/////////////////////////////////////////////////////////////////////////////////////////////////exit
        }
    }else                 //不在epoll中，添加
    {
        if(epoll_ctl(epfd,EPOLL_CTL_ADD,_channel->fd(),&tmpEv))
        {
            printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
            perror(":");
            exit(-1);/////////////////////////////////////////////////////////////////////////////////////////////////exit
        }
        _channel->SetInEpoll();//设置inEpoll
    }
}

//清除Channel
void Epoll::removeChannel(Channel *_channel)
{
    if(_channel->inEpoll())//已在epoll中，修改
    {
        if(epoll_ctl(epfd,EPOLL_CTL_DEL,_channel->fd(),0))
        {
            printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
            perror(":");
            exit(-1);/////////////////////////////////////////////////////////////////////////////////////////////////exit
        }
    }
}
std::vector<Channel*>Epoll::loop(int time)              //等待事件发生，并将发生的事件转换成Channel*
{
    std::vector<Channel*>re(0);
    int num=epoll_wait(epfd,evs,maxsize,time);
    if(num==0)             //-1超时
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(",epoll_wait超时:");
        exit(-1);/////////////////////////////////////////////////////////////////////////////////////////////////exit
    }else if(num==-1)        //0出错
    {
        printf("文件%s的%d行的[%s]函数出错", __FILE__, __LINE__, __func__);
        perror(",epoll_wait出错:");
        exit(-1);/////////////////////////////////////////////////////////////////////////////////////////////////exit
    }

    for(int i=0;i<num;i++)  //将发生的事件转换成Channel*
    {
        Channel *tmpChannel=(Channel*)evs[i].data.ptr;  //取出Channel，这些Channel来自Connection，归Connection管理生命周期
        tmpChannel->SetRevent(evs[i].events);           //手动更新Channel实际发生的事件
        re.push_back(tmpChannel);
    }
    return re;
}