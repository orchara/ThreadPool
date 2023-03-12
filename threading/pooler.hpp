#pragma once

#include <functional>
#include <thread>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <list>


class ThreadPool{
private:
    size_t size;
    std::atomic<bool> is_runing;
    std::vector<std::thread> threads;
    std::list<std::function<void()>> queue;
    std::mutex queue_lock;
    std::condition_variable queue_check;


public:
    ThreadPool(size_t s){
        is_runing = true;
        size = s;
        for(size_t i = 0; i < s; i++){
            
            threads.emplace_back(&ThreadPool::work, this);
        }
    }

    ~ThreadPool(){
        //Join();
    }


    template <typename _Fn, typename... _Args>
    size_t add_task(_Fn&& func, _Args&&... ag){
        auto tmp = std::bind(std::forward<_Fn>(func), std::forward<_Args>(ag)...);
        std::lock_guard<std::mutex> q_lock(queue_lock);
        queue.push_back([&func, &ag...](){std::bind(std::forward<_Fn>(func), std::forward<_Args>(ag)...)();});
        queue_check.notify_one();
        return 0;
    }


    void stop(){
        is_runing = false;
        Join();
    }
    
private:
    void Join(){
        for(auto& trd : threads){
            if(trd.joinable()){
                trd.join();
            }
        }
    }

    void work(){
        while(is_runing){
            std::unique_lock<std::mutex> q_lock(queue_lock);
            queue_check.wait(q_lock, [this](){return !queue.empty();});
            if(!queue.empty()){
                auto task = std::move(queue.front());
                queue.pop_front();
                q_lock.unlock();

                task();
            }
        }
        return;
    }
};