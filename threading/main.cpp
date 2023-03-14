#include <iostream>
#include <exception>
#include <type_traits>
#include <chrono> 
#include "pooler.hpp"



void twice(int x, int& res){
    std::this_thread::sleep_for (std::chrono::seconds(2));
    res = x * 2;
    return;
}

int add_five(int x, int& res){
    std::this_thread::sleep_for (std::chrono::seconds(3));
    res = x + 5;
    return res;
}


int main(){
    try{
        std::vector<int> r(5, 0);
        // add_five(5, r[0]);
        ThreadPool trd(2);
        trd.error_handle();
        std::function<void(int, int&)> t_square = [] (int x, int& res) {
            res = x * x;
            std::cout << "lambda in work\n";
            };        
        
        
        trd.add_task(add_five, 10, std::ref(r[2]));
        trd.add_task(t_square, 3, std::ref(r[0]));
        trd.add_task(t_square, 30, std::ref(r[1]));
        size_t idx = trd.add_task(twice, 20, std::ref(r[3]));

        auto t_div = 
            [&trd] (int x, int& res, size_t id) {
                trd.wait(id);
                // std::cout << "x: "<< x <<std::endl;
                res = x / 2;
                throw std::runtime_error("wowowow");
                
            };
        
        trd.add_task(t_div, std::ref(r[3]), std::ref(r[4]), idx);

        trd.wait_all();
        
        
        for(auto i : r){
            std::cout << i << std::endl;
        }
        
        std::this_thread::sleep_for (std::chrono::seconds(20));
        trd.stop();
        return 0;
    }
    catch (std::exception& err){
        std::cout << err.what();
    }
}

