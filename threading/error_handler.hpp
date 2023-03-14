#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <exception>
#include <atomic>

class ErrorHandler {
public:
    std::condition_variable work_state;
private:
    std::list<std::exception_ptr> exceptions;
    std::mutex mtx;
    std::atomic<bool> is_runing {false}, work_done {false};
    std::vector<std::string> err_buf;
    

public:
    void handler(){
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
                err_buf.push_back(std::string("ex.what(): ") + std::string(ex.what()) + "\n");
            }
        }
        work_done = true;
        work_state.notify_all();
    }


    void stop() {
        is_runing = false;
    }


    void push(std::exception_ptr ex) {
        if(is_runing){
            std::lock_guard<std::mutex> mu_lock(mtx);
            exceptions.push_back(ex);
        }
    }


    bool is_done() {
        return work_done;
    }


    std::vector<std::string> get_log(){
        return std::move(err_buf);
    }

private:
    std::exception_ptr pop() {
        std::lock_guard<std::mutex> mu_lock(mtx);
        std::exception_ptr ex = exceptions.front();
        exceptions.pop_front();
        return ex;
    }
};