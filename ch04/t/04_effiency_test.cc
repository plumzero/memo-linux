
#include <cstdint>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <map>
#include <sys/time.h>


/**
 * 不要问为什么这个测试程序看起这么奇葩，既然是测试程序，奇葩一点又有什么关系。
 */

std::mutex mtx;

void add0(uint64_t& num, uint64_t& sum)
{
    while (true) {
        num += 1;
        if (num < 1000000) {
            
        } else {
            break;
        }
    }
}

void add1(uint64_t& num, uint64_t& sum)
{
    while (true) {
        std::lock_guard<std::mutex> locker(mtx);
        num += 1;
        if (num < 1000000) {
        
        } else {
            break;
        }
    }
}

void add2(uint64_t& num, uint64_t& sum)
{
    while (true) {
        int tmp = __sync_add_and_fetch(&num, 1);
        if (tmp < 1000000) {
        
        } else {
            break;
        }
    }
}


int main()
{
    uint64_t sum = 0;
    uint64_t num = 0;
    std::vector<std::thread> vec;
    int i;
    
    auto b = std::chrono::high_resolution_clock::now();
    
    for (i = 0; i < 20; i++) {
        std::thread th = std::thread(add2, std::ref(num), std::ref(sum));
        vec.emplace_back(std::move(th));
    }
    
    std::for_each(vec.begin(), vec.end(), std::mem_fn(&std::thread::join));
    
    auto e = std::chrono::high_resolution_clock::now();
    
    double cost = std::chrono::duration_cast<std::chrono::microseconds>(e - b).count();
    
    std::cout << "num=" << num << ". spend " << cost << " microseconds." << std::endl;
    
    return 0;
}