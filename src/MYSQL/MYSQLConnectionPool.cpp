#include"MYSQLConnectionPool.h"


//构造函数（ip,端口，用户名，密码，数据库，连接数）
MYSQLConnectionPool::MYSQLConnectionPool(const std::string& _host,int _port,//ip，port
                                const std::string& _user,const std::string& _passwd,//用户名，密码
                                const std::string& _dataBase,//数据库
                                size_t _maxSize,size_t _minSize,//最大连接数，最小连接数/最大空闲连接数
                                int _idleCheckInterval,int _connectionTimeOut)://空闲连接检查间隔，连接超时时间
                //初始化列表
                host_(_host),port_(_port),user_(_user),passwd_(_passwd),
                dataBase_(_dataBase),
                maxSize_(_maxSize),minSize_(_minSize),
                idleCheckInterval_(_idleCheckInterval),connectionTimeOut_(_connectionTimeOut),
                backGroundThread_([this]{BackGroundTask();})//后台线程
{
    // //初始化连接
    // for(size_t i=0;i<minSize_;i++)
    // {
    
    //     try//初始化连接
    //     {
    //         //初始化无需加锁
    //         auto session=CreateNewConnection();
    //         pool_.push(std::move(session));
    //     }catch(const std::exception& e)//正常异常
    //     {
    //         std::cerr<<"MYSQL连接池初始化发生错误："<<e.what()<<std::endl;
    //     }
    // }
}
//初始化连接
bool MYSQLConnectionPool::Init()
{
    //初始化连接
    for(size_t i=0;i<minSize_;i++)
    {
    
        try//初始化连接
        {
            //初始化无需加锁
            auto session=CreateNewConnection();
            pool_.push(std::move(session));

        }catch(const std::exception& e)//正常异常
        {
            std::cerr<<"MYSQL连接池初始化发生错误："<<e.what()<<std::endl;
            return false;
        }
    }
    return true;
}
//析构函数
MYSQLConnectionPool::~MYSQLConnectionPool()
{
    //停止后台线程
    stopBackGroungThread_=true;
    poolCondition_.notify_all();
    if(backGroundThread_.joinable())backGroundThread_.join();

    std::lock_guard<std::mutex> lock(connectionMutex_);//加锁，避免关闭数据库时还有连接操作数据库
    while(!pool_.empty())
    {
        try
        {
            //获取底层连接
            DBConnection connection=std::move(pool_.front());
            if(connection.isValid())(*connection).close();
        }catch(...)
        {
            //先忽略
        }
        pool_.pop();
    }
    currentNum_=0;
}
//创建新连接
DBConnection MYSQLConnectionPool::CreateNewConnection()
{
    try
    {
        //创建连接
        upDBSession connection=upDBSession (new mysqlx::Session(host_,port_,user_,passwd_,dataBase_));
        currentNum_+=1;
        return DBConnection(shared_from_this(),std::move(connection));
    }catch(const std::exception& e)//抛出异常交给调用方（GetConnection处理）
    {
        
        throw std::runtime_error("MYSQL连接错误："+std::string(e.what()));
    }catch(...)
    {
        throw std::runtime_error("MYSQL连接错误：未知错误");
    }
}

//得到一个连接
DBConnection MYSQLConnectionPool::GetConnection()
{
    std::unique_lock<std::mutex> lock(connectionMutex_);//加锁保证不错过通知，条件变量所以用unique_lock

    if(poolCondition_.wait_for(lock,std::chrono::seconds(connectionTimeOut_),
        [this]{return !pool_.empty()||currentNum_<maxSize_;})==false)//有空闲连接或能创建新连接
    {
        //超时
        throw std::runtime_error("请求连接超时\n");
    }
    //有空闲连接
    if(!pool_.empty())
    {
        DBConnection connection=std::move(pool_.front());
        pool_.pop();
        return connection;
    }
    //无空闲连接但能创建新连接
    if(currentNum_<maxSize_)
    {
        lock.unlock();//创建新连接开销大，解锁（CreateNewConnection线程安全）
        try
        {
            DBConnection session=CreateNewConnection();
            return session;
        }catch(const std::exception& e)
        {
            throw std::runtime_error("创建新连接失败："+std::string(e.what()));
        }
    }
    //不应执行到这里
    throw std::runtime_error("无可用连接（GetConnection出错）");    
}
//归还连接
bool MYSQLConnectionPool::ReleaseConnection(DBConnection _connection)
{
    if(_connection.isValid()==false)
    {
        currentNum_-=1;
        poolCondition_.notify_one();//可创建新线程
        return false;
    }
    //连接入池
    {
        std::lock_guard<std::mutex> lock(connectionMutex_);//加锁，因为操作pool_
        pool_.push(std::move(_connection));
    }

    //通知有可用线程
    poolCondition_.notify_one();
    return true;
}
//归还连接(直接归还资源)
void MYSQLConnectionPool::ReleaseConnection(upDBSession _session)
{
    try
    {
        //验证有效性
        _session->sql("select 1").execute();
        //连接入池
        {
            std::lock_guard<std::mutex> lock(connectionMutex_);//加锁，因为操作pool_
            pool_.push(DBConnection(shared_from_this(),std::move(_session)));
            std::cerr<<"自动归还连接成功\n";
        }
    }
    catch(...)
    {
        currentNum_-=1;
        std::cerr<<"自动归还连接失败\n";
    }
    
}
//总连接数
size_t MYSQLConnectionPool::size()
{
    std::lock_guard<std::mutex> lock(connectionMutex_);//加锁，因为涉及currentNum
    return currentNum_;

}
//空闲连接数
size_t MYSQLConnectionPool::vaildNum()
{
    std::lock_guard<std::mutex> lock(connectionMutex_);//加锁，因为涉及pool_
    return pool_.size();
}

//后台线程，维持连接数及检查超时连接
void MYSQLConnectionPool::BackGroundTask()
{
    while(!stopBackGroungThread_)
    {
        //加锁，因为操作连接
        std::unique_lock<std::mutex> lock(connectionMutex_);

        //休眠空闲间隔，使用条件变量，可以唤醒
        if(poolCondition_.wait_for(lock,std::chrono::seconds(idleCheckInterval_),
        [this]{return stopBackGroungThread_.load();})==true)//等于true说明通知+stopBackGroungThread_w为true
        {
            break;//停止后台线程
        };

        //检查无效连接
        std::queue<DBConnection> newPool;//存放有效连接的新队列
        while(!pool_.empty())
        {
            DBConnection connection=std::move(pool_.front());
            pool_.pop();

            if(connection.isValid())newPool.push(std::move(connection));
            else 
            {
                currentNum_-=1;
            }
        }
        pool_=std::move(newPool);//转移队列,有效连接入池


        //检查空闲连接上限
        while(pool_.size()>minSize_)
        {
            try
            {
                DBConnection connection=std::move(pool_.front());
                if(connection.isValid())(*connection).close();
            }catch(...)
            {
                //忽略，后续优化
            }
            pool_.pop();
            currentNum_-=1;
        }


        //检查总连接下限
        while(pool_.size()<minSize_)
        {
            try
            {
                pool_.push(std::move(CreateNewConnection()));
            }catch(std::exception& e)
            {
                std::cerr<<"后台线程创建新连接失败"<<e.what()<<std::endl;
                break;//避免无限循环
            }
        }
    }
}