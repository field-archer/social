#pragma once
#include<string>
#include<queue>
#include<exception>
#include<iostream>
#include<mutex>
#include<memory>
#include<condition_variable>
#include<chrono>
#include<atomic>
#include<thread>
#include <mysqlx/xdevapi.h>
#include"DBConnection.h"
// using spmysqlConnection=std::shared_ptr<mysqlx::Session>;
class MYSQLConnectionPool;
class DBConnection;
using spDBPool=std::shared_ptr<MYSQLConnectionPool>;
using upDBSession=std::unique_ptr<mysqlx::Session>;


class MYSQLConnectionPool:public std::enable_shared_from_this<MYSQLConnectionPool>
{
private:
    //连接参数
    std::string host_;                  //主机/ip
    int port_;                          //端口
    std::string user_;                  //用户名
    std::string passwd_;                //密码
    std::string dataBase_;              //所用数据库

    //连接池配置
    std::atomic<size_t> currentNum_;                 //当前连接数目（空闲+已借出）
    size_t maxSize_;                    //最大连接数-----总连接上限
    size_t minSize_;                    //最小连接数-----总连接下限，空闲连接上限
    int idleCheckInterval_;             //空闲连接检查间隔
    int connectionTimeOut_;             //连接超时时间

    //连接池
    std::condition_variable poolCondition_;//连接池条件变量
    std::queue<DBConnection> pool_;        //空闲连接
    std::mutex connectionMutex_;        //连接锁
    //后台线程
    std::atomic<bool> stopBackGroungThread_;//停止后台线程
    std::thread backGroundThread_;      //后台线程
    void BackGroundTask();              //后台线程，维持连接数及检查超时连接


    DBConnection CreateNewConnection();                       //创建新连接                        
public:
    MYSQLConnectionPool(const std::string& _host,int _port,
                    const std::string& _user,const std::string& _passwd,
                    const std::string& _dataBase,
                    size_t _maxSize,size_t _minSize,
                    int _idleCheckInterval,int _connectionTimeOut);         //构造函数（ip,端口，用户名，密码，数据库，连接数）
    ~MYSQLConnectionPool();                                                     //析构函数（无用）

    bool Init();                                                                //初始化连接

    DBConnection GetConnection();                                               //得到一个连接
    bool ReleaseConnection(DBConnection _connection);                           //归还连接
    void ReleaseConnection(upDBSession _session);                               //归还连接(直接归还资源)

    size_t size();                                                    //总连接数
    size_t vaildNum();                                                //空闲连接数

};