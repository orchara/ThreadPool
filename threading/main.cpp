#include <iostream>
#include <exception>
#include <type_traits>
#include <chrono> 
#include "pooler.hpp"

void twice(int x, int& res){
    std::this_thread::sleep_for (std::chrono::seconds(6));
    res = x * 2;
    return;
}

int add_five(int x, int& res){
    std::this_thread::sleep_for (std::chrono::seconds(20));
    res = x + 5;
    return res;
}


int main(){
    try{
        
        std::function<void(int, int&)> t_square = [] (int x, int& res) {
            res = x * x;
            std::cout << "lambda in work\n";
            };
            
        ThreadPool trd(2);
        std::vector<int> r(4, 0);
        trd.add_task(add_five, 10, std::ref(r[2]));
        trd.add_task(t_square, 3, std::ref(r[0]));
        trd.add_task(t_square, 30, std::ref(r[1]));
        trd.add_task(twice, 20, std::ref(r[3]));

        trd.wait_all();
        
        
        for(auto i : r){
            std::cout << i << std::endl;
        }

        trd.stop();
        return 0;
    }
    catch (std::exception err){
        std::cout << err.what();
    }
}

