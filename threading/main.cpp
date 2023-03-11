#include <iostream>
#include <exception>

#include "pooler.hpp"


int main(){
    try{
        std::function<void(int)> func = [] (int x) {std::cout << x << std::endl;};

        ThreadPool trd(1);

        trd.add_task(func, 3000);
        //trd.add_task(func, 30);
        trd.start();

        return 0;
    }
    catch (std::exception err){
        std::cout << err.what();
    }
}

