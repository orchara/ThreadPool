#pragma once

#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <vector>
#include <list>
#include <set>
#include "error_handler.hpp"


class ThreadPool{
private:
    size_t size;
    std::atomic<bool> is_runing;
    std::atomic<size_t> id {0};
    std::vector<std::thread> threads;
    std::set<size_t> complete_task_idx;
    std::list<std::pair<std::function<void()>, size_t>> queue;
    std::mutex queue_lock, state_lock, err_lock;
    std::condition_variable queue_check, work_state;
    ErrorHandler thread_exceptions;


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
        size_t t_id = get_id();
        std::lock_guard<std::mutex> q_lock(queue_lock);
        queue.push_back({[tmp](){tmp();}, t_id});
        queue_check.notify_one();
        return t_id;
    }


    void wait_all(){
        std::unique_lock<std::mutex> s_lock(state_lock);
        work_state.wait(s_lock, [this]()->bool{
            // std::lock_guard<std::mutex> q_lock(queue_lock);
            return queue.empty();
        });
    }


    void wait(size_t t_id){
        std::unique_lock<std::mutex> s_lock(state_lock);
        work_state.wait(s_lock, [this, &t_id](){
            return complete_task_idx.find(t_id) != complete_task_idx.end();
        });
        
    }    

    void error_handle() {
        // std::thread er_h();
        threads.push_back(std::thread(&ErrorHandler::start, &thread_exceptions));
    }

    void stop(){
        is_runing = false;
        thread_exceptions.stop();
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
                queue_check.wait(q_lock, [this](){return !queue.empty() || !is_runing;});
                
                if(!queue.empty()){
                    
                        auto task = std::move(queue.front());
                        queue.pop_front();
                        std::cout << "in work befeore try\n";
                    try {
                        std::cout << "in work inside try\n";
                        task.first();
                    } catch (const std::exception& ex) {
                        std::cout << "in work inside catch\n";
                        thread_exceptions.push(ex);
                    }
                        q_lock.unlock();
                        q_lock.release();
                        std::unique_lock<std::mutex> q_lock(state_lock);
                        complete_task_idx.insert(std::move(task.second));
                        work_state.notify_all();
                    
                }
            
        }
        return;
    }

    size_t get_id(){
        size_t res = id.load(std::memory_order_relaxed);
        id.store(res + 1, std::memory_order_relaxed);
        return res;
    }
};