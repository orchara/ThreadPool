#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <exception>
#include <atomic>

class ErrorHandler {
private:
    std::list<std::exception_ptr> exceptions;
    std::mutex mtx;
    std::atomic<bool> is_runing {false};

public:
    void start(){
        is_runing = true;
        while(is_runing || !exceptions.empty()){
            while(exceptions.empty() && is_runing){
                std::this_thread::yield();
            }
            try {
                if(!exceptions.empty()) {
                    std::exception_ptr p = pop();
                    std::rethrow_exception(p);
                }
            } catch (std::exception& ex) {
                std::cerr << "ex.what(): " << ex.what() << std::endl;
            }
        }
    }


    void stop() {
        is_runing = false;
    }


    void push(std::exception_ptr ex) {
        std::cout << "push\n";
        if(is_runing){
            std::lock_guard<std::mutex> mu_lock(mtx);
            exceptions.push_back(ex);
        }
    }

private:
    std::exception_ptr pop() {
        std::lock_guard<std::mutex> mu_lock(mtx);
        std::exception_ptr ex = exceptions.front();
        exceptions.pop_front();
        return ex;
    }
};