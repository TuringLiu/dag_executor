#include "thread_pool.h"
#include "iostream"

ThreadPool::ThreadPool(const int& worer_num=4, const int& task_size=10): worker_num_(worer_num), tasks_size_(task_size)
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
    stop();
    for(int i = 0; i < worker_num_; i++)
    {
        if(threads_[i].joinable())
        {
            std::cout << "still alive worker " << std::this_thread::get_id() << std::endl;
            threads_[i].join();
        }
        delete workers_[i];
    }
}

void ThreadPool::add_task(std::function<void()> f)
{
    if(stop_ == true) 
    {
        std::cout << "thread pool has been stopped! add_task failed!" << std::endl;
        return;
    }
    
    // todo: 负载均衡
    if(!workers_[idx_]->add_task(f))
    {
        // todo: 当任务队列溢出时，放入redis缓存
    }
    idx_ = (idx_ + 1) % worker_num_;
}

std::future<int> ThreadPool::add_task_async(std::function<void()> f)
{
    std::packaged_task<int()> task([&f](){
        f();
        return 0;
    });

    std::future<int> res = task.get_future();
    add_task([&task](){
        task();
    });
    return res;
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
    // notify all workers to stop
    for(auto& worker: workers_)
    {
        if(worker != nullptr)
            worker->task_cond_.notify_one();
    }
    stop_cond_.wait(lock, [this](){
        return this->stop_num_ == this->worker_num_;
    });
    // std::cout << "thread pool stop!" << std::endl;
}

void ThreadPool::thread_over_()
{
    stop_num_++;
    if(stop_num_ == worker_num_) stop_cond_.notify_one(); 
}


Worker::Worker(const int& tasks_size, const int& id): tasks_size_(tasks_size), worker_id_(id)
{
}

bool Worker::add_task(std::function<void()> f)
{
    std::unique_lock<std::mutex> lock(task_mtx_);
    if(static_cast<int>(tasks_queue_.size()) >= tasks_size_) 
    {
        std::cout << "tasks_queue_ overflow!" << std::endl;
        return false;
    }
    
    tasks_queue_.push(std::forward<std::function<void()>>(f));
    if(tasks_queue_.size() == 1) task_cond_.notify_one();

    return true;
}

void Worker::work_()
{
    while(!thread_pool_->stop_ || !tasks_queue_.empty())
    {
        std::unique_lock<std::mutex> lock(task_mtx_);
        if(static_cast<int>(tasks_queue_.size()) == 0) 
        {
            lock.unlock();
            work_steal_();
            lock.lock();
            if(static_cast<int>(tasks_queue_.size()) == 0) 
            {
                task_cond_.wait(lock, [this](){
                    return !this->tasks_queue_.empty() || this->thread_pool_->stop_;
                });
            }
            if(thread_pool_->stop_ && tasks_queue_.empty()) break;
        }

        auto f = tasks_queue_.front();
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

