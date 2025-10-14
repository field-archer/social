#include"PostDAO.h"

//构造函数
PostDAO::PostDAO(spDBPool _dbPool):dbPool_(_dbPool)
{

}
//拷贝构造函数
PostDAO::PostDAO(const PostDAO& _postDAO):dbPool_(_postDAO.dbPool_)
{

}
//发表贴子，发表后Post所存贴子内容丢失，返回贴子id/-1
int PostDAO::PublishPost(std::unique_ptr<Post> _post)
{
    try
    {
        //获取连接
        DBConnection connection=dbPool_->GetConnection();
        //操作数据库
        mysqlx::SqlResult result=(*connection).sql("insert into post (content,user_id) values(?,?)")
                        .bind(*(_post->GetContent_unique()),_post->GetUserId()).execute();
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
