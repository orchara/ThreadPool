#pragma once

#include <functional>
#include <thread>
#include <vector>
#include <list>


class ThreadPool{
private:
    size_t size;
    bool is_runing;
    std::vector<std::thread> threads;
    std::list<std::thread> queue;

public:
    ThreadPool(size_t s){
        size = s;
    }


    template <typename _Fn, typename... __Args>
    size_t add_task(_Fn&& func, __Args&&... ag){
        std::thread trd(std::forward<_Fn>(func), std::forward<__Args>(ag)...);
        if(threads.size() < size){
            threads.push_back(std::move(trd));
        } else {
            queue.push_back(std::move(trd));
        }
        
        return 0;
    }


    void start(){
        Join();
    }
    
private:
    void Detach(){
        for(auto& trd : threads){
            if(trd.joinable()){
                trd.detach();
            }
        }
    }

    void Join(){
        for(auto& trd : threads){
            if(trd.joinable()){
                trd.join();
            }
        }
    }
};