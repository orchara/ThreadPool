#pragma once

#include <condition_variable>

class TaskStorage {
public:
    //...
private:
    //task
    std::condition_variable check_done;
    //result
public:
    template<typename _Fn, typename... T>
    TaskStorage(_Fn func, T... ag){
        // set_task(func, ag...)
    }
    // auto get_result()
    // void wait()
    // T get_task()

private:
    // template<typename _Fn, typename... T>
    // class Task{
        // void set_task(_Fn func, T... ag) { //all
    //  }
    // };

    // template<typename... T>
    // class Task <std::function<void(T...)>, T...> {
        // void set_task(std::function<void(T...)> func, T... ag) //void
};