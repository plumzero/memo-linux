
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>

#include "hpallocator.h"
#include "utils.h"

int main()
{
    const size_t sz =  2 * 1024 * 1024;  // 2M 的长度
    using Type = double;

    // 默认页尺寸
    {
        std::vector<Type> vchars;
        vchars.reserve(sz * sizeof(Type));

        for (size_t i = 0; i < sz; i++) {
            vchars.emplace_back('a' + i % 26);
        }

        randev rd(0, sz - 1);

        std::this_thread::sleep_for(std::chrono::seconds(2));    // 让 CPU 放松一会儿
        auto b1 = std::chrono::steady_clock::now();

        for (int j = 0; j < 40960; j++) {    // 修改 40960 次
            int index = rd.given();
            vchars[index] = std::toupper(vchars[index]);
        }

        auto e1 = std::chrono::steady_clock::now();
        std::cout << "缺省尺寸耗时(微秒): " << timediff<std::chrono::microseconds>(b1, e1) << std::endl;
    }


    // 大页尺寸
    {
        std::vector<Type, hpallocator<Type>> vchars;
        vchars.reserve(sz * sizeof(Type));

        for (size_t i = 0; i < sz; i++) {
            vchars.emplace_back('a' + i % 26);
        }

        randev rd(0, sz - 1);

        std::this_thread::sleep_for(std::chrono::seconds(2));    // 让 CPU 放松一会儿
        auto b1 = std::chrono::steady_clock::now();

        for (int j = 0; j < 40960; j++) {    // 修改 40960 次
            int index = rd.given();
            vchars[index] = std::toupper(vchars[index]);
        }

        auto e1 = std::chrono::steady_clock::now();
        std::cout << "大页尺寸耗时(毫秒): " << timediff<std::chrono::microseconds>(b1, e1) << std::endl;
    }

    std::cout << "=== over ===" << std::endl;
    
    getchar();

    return 0;
}