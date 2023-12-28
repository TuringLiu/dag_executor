#include "thread_pool.h"
#include "iostream"

ThreadPool::ThreadPool()
{
    ThreadPool(4, 10);
}

ThreadPool::ThreadPool(const int& worer_num, const int& task_size): worker_num_(worer_num), tasks_size_(task_size)
{
    for(int i = 0; i < worker_num_; i++) 
    {
        workers_.emplace_back(new Worker(task_size, i));
        workers_[i]->thread_pool_ = this;
    }

    run_();
}

ThreadPool::~ThreadPool()
{
    for(int i = 0; i < worker_num_; i++)
    {
        delete workers_[i];
    }
}

void ThreadPool::add_task(std::function<void()> f)
{
    if(stop_ == true) return;
    
    // todo: 负载均衡
    if(!workers_[idx_]->add_task(std::move(f)))
    {
        // todo: 当任务队列溢出时，放入redis缓存
    }
    std::unique_lock<std::mutex> lock(mtx_);
    idx_ = (idx_ + 1) % worker_num_;
}

void ThreadPool::run_()
{
    // 调度work执行任务
    for(auto& worker: workers_)
    {
        threads_.emplace_back([&worker](){
            worker->work_();
        });
    }
}

void ThreadPool::stop()
{
    stop_ = true;
    std::unique_lock<std::mutex> lock(mtx_);
    stop_cond_.wait(lock, [this](){
        return this->stop_num_ == this->worker_num_;
    });
}

void ThreadPool::thread_over_()
{
    std::unique_lock<std::mutex> lock(mtx_);
    stop_num_++;
    if(stop_num_ == worker_num_) stop_cond_.notify_one(); 
}


Worker::Worker(const int& tasks_size, const int& id): tasks_size_(tasks_size), worker_id_(id)
{
}

bool Worker::add_task(std::function<void()>&& f)
{
    std::unique_lock<std::mutex> lock(task_mtx_);
    if(static_cast<int>(tasks_queue_.size()) >= tasks_size_) return false;
    
    tasks_queue_.push(std::forward<std::function<void()>>(f));

    return true;
}

void Worker::work_()
{
    while(!thread_pool_->stop_)
    {
        std::unique_lock<std::mutex> lock(task_mtx_);
        if(static_cast<int>(tasks_queue_.size()) == 0) 
        {
            lock.unlock();
            work_steal_();
            continue;
        }
        auto& f = tasks_queue_.front();
        tasks_queue_.pop();
        lock.unlock();
        f();
    }  
    thread_pool_->thread_over_();
}

void Worker::work_steal_()
{
    // todo: work stealing from other worker or redis
}

