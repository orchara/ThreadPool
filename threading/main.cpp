#include <iostream>
#include <exception>
#include <type_traits>
#include "pooler.hpp"


int result = 0;
template <typename F>
struct test{
    F func;
};

void twice(int x, int& res){
    res = x * 2;
}

int add_five(int x, int& res){
    res = x + 5;
    return res;
}

    template <typename _Fn, typename... _Args>
    void fu(bool ret, _Fn&& func, _Args&&... ag){
        
        std::function<void()> fc;
        
        auto tmp = std::bind(std::forward<_Fn>(func), std::forward<_Args>(ag)...);
        //if(ret){
        //    fc = [tmp](){auto res = tmp();};
        //} else {
            fc = [tmp](){tmp();};
        //}
        fc();
}

int main(){
    try{
        
        std::function<void(int, int&)> func = [] (int x, int& res) {res = x * x;};
        
        //auto tmp = std::bind<int>(t_func_2, 10);
        //auto tmp = std::bind(t_func_1, 10);
        
        // fu(true, t_func_2, 10);
        // fu(false, t_func_1, 20);
        // typedef std::result_of<decltype(t_func_2)&(int)>::type ret_type;
        // auto tmp = std::bind<ret_type>(t_func_2, 50);
        // int test = tmp();
        std::cout << "result is: " << result << std::endl;

        ThreadPool trd(1);
        std::vector<int> r(4, 0);
        // trd.add_task(func, 3000, std::ref(res[0]));
        // trd.add_task(func, 30, std::ref(res[1]));
        trd.add_task(add_five, 10, std::ref(r[2]));
        trd.add_task(twice, 20, std::ref(r[3]));
        //trd.start();
        trd.stop();

        // std::thread trd1(add_five, 10, std::ref(res[2]));
        // trd1.join();

        for(auto i : r){
            std::cout << i << std::endl;
        }
        return 0;
    }
    catch (std::exception err){
        std::cout << err.what();
    }
}

