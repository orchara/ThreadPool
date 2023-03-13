#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <exception>
#include <atomic>

class ErrorHandler {
private:
    std::list<std::exception> exceptions;
    std::mutex mtx;
    std::atomic<bool> is_runing {false};

public:
    void start(){
        is_runing = true;
        while(is_runing || !exceptions.empty()){
            while(exceptions.empty() && is_runing){
                std::this_thread::yield();
            }
            std::cout << "before try\n" << exceptions.size() << std::endl;
            try {
                if(!exceptions.empty()) {
                    std::exception_ptr p = std::make_exception_ptr(pop());
                    std::cout << "exception throw\n";
                    std::rethrow_exception(p);
                }
            } catch (const std::exception& ex) {
                std::cerr << ex.what() << std::endl;
                std::cout << "in catch\n";
            }
            std::cout << "after catch\n";    
        }
        std::cout << "stop\n";
    }


    void stop() {
        is_runing = false;
    }


    void push(std::exception ex) {
        std::cout << "push\n";
        if(is_runing){
            std::lock_guard<std::mutex> mu_lock(mtx);
            exceptions.push_back(ex);
        }
    }

private:
    std::exception pop() {
        std::lock_guard<std::mutex> mu_lock(mtx);
        std::exception ex = exceptions.front();
        exceptions.pop_front();
        return ex;
    }
};