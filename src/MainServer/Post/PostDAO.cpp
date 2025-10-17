#include"PostDAO.h"

//构造函数
PostDAO::PostDAO(spDBPool _dbPool):dbPool_(_dbPool)
{

}
//拷贝构造函数
PostDAO::PostDAO(const PostDAO& _postDAO):dbPool_(_postDAO.dbPool_)
{

}
//发表贴子，发表后Post所存贴子内容丢失，返回贴子id/-1(未知错误)
int PostDAO::PublishPost(Post _post)
{
    try
    {
        //获取连接
        DBConnection connection=dbPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*connection).sql("insert into post (content,user_id) values(?,?)")
                        .bind(*(_post.GetContent_unique()),_post.GetUserId()).execute();
        //获取贴子id
        int postId=result.getAutoIncrementValue();
        //根据影响行数判断成功失败
        if(result.getAffectedItemsCount()>0)return postId;
        else return -1;
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("发表帖子失败:"+std::string(e.what()));
    }
    
}
//删除贴子
bool PostDAO::DeletePost(int _postId)
{
    try
    {
        //获取连接
        DBConnection connection=dbPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*connection).sql("delete from post where post_id=?")
                            .bind(_postId).execute();
        //根据影响行数判断成功失败
        return result.getAffectedItemsCount()>0;
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("删除帖子失败:"+std::string(e.what()));
    }
}
//返回连接池
spDBPool PostDAO::GetDBPool()
{
    return dbPool_;
}
//查看某用户所有贴子
std::vector<Post> PostDAO::SelectPostsByUserId(int _userId)
{
    try
    {
        //获取连接
        DBConnection connection=dbPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*connection).sql("select post_id,content,create_time from post where user_id=?")
                            .bind(_userId).execute();
        //获取数据
        auto rows=result.fetchAll();
        std::vector<Post> returnPosts;
        for(const mysqlx::Row& row:rows)
        {
            int postId=row[0].get<int>();
            //get<>()返回临时对象，无需move，编译器自动优化为移动赋值
            std::string content=row[1].get<std::string>();
            std::string createTime=row[2].get<std::string>();
            //获取unique_ptr指针
            std::unique_ptr<std::string> contentPtr=std::make_unique<std::string>(std::move(content));
            returnPosts.push_back(std::move(Post(postId,std::move(contentPtr),_userId,std::move(createTime))));
        }
        //返回所有贴子
        return returnPosts;
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("查看用户帖子失败:"+std::string(e.what()));
    }
}
