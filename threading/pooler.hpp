#pragma once

#include <functional>
#include <thread>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <atomic>
#include <list>


class ThreadPool{
private:
    size_t size;
    std::atomic<bool> is_runing;
    std::vector<std::thread> threads;
    std::list<std::function<void()>> queue;
    std::mutex queue_lock, state_lock;
    std::condition_variable queue_check, work_state;


public:
    ThreadPool(size_t s){
        is_runing = true;
        size = s;
        for(size_t i = 0; i < s; i++){
            threads.emplace_back(&ThreadPool::work, this);
        }
    }

    ~ThreadPool(){
        Join();
    }


    template <typename _Fn, typename... _Args>
    size_t add_task(_Fn&& func, _Args&&... ag){
        auto tmp = std::bind(std::forward<_Fn>(func), std::forward<_Args>(ag)...);
        std::lock_guard<std::mutex> q_lock(queue_lock);
        queue.push_back([tmp](){tmp();});
        queue_check.notify_one();
        return 0;
    }


    void wait_all(){
        std::unique_lock<std::mutex> s_lock(state_lock);
        work_state.wait(s_lock, [this]()->bool{
            std::lock_guard<std::mutex> q_lock(queue_lock);
            return queue.empty();
        });
    }

    void stop(){
        is_runing = false;
        queue_check.notify_all();        
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
            queue_check.wait(q_lock);
            
            if(!queue.empty()){
                auto task = std::move(queue.front());
                queue.pop_front();
                q_lock.unlock();
                task();
                work_state.notify_all();
            }
        }
        std::cout << "thread " << std::this_thread::get_id() << " closed up\n";
        return;
    }
};