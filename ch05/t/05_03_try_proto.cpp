
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>

#include "hpallocator.h"
#include "utils.h"

#include "struct.pb.h"

int main()
{
    const size_t sz = 2 * 1024 * 60;    // 2M 的长度
    const int total = 40960;            // 要修改的次数

    using Type = mam::TradeOrder;
    using Ptr = Type*;

    std::cout << "元素长度: " << sizeof(Type) << " 分配内存: " << sz * sizeof(Type) << std::endl;

    // 随机数生成器可能会造成计算波动，不使用了
    randev rd(0, sz - 1);

    // 默认页尺寸
    {
        std::vector<Type> vec;
        vec.reserve(sz * sizeof(Type));

        for (size_t i = 0; i < sz; i++) {
            vec.emplace_back(Type());
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));    // 让 CPU 放松一会儿
        auto b1 = std::chrono::steady_clock::now();

        for (int j = 0; j < total; j++) {
            // int index = rd.given();
            int index = ((sz / total) * j * 97) % sz;
            vec[index].set_reqid(j);
            vec[index].set_symbol(std::to_string(j));
            vec[index].set_orderprice(j * 1000);
            vec[index].set_ordervolume(j * 100);
            vec[index].set_force(j % 2);
            vec[index].set_createtime(j * 1000);
            vec[index].set_closetime(j * 10000 + j * 100);
            vec[index].set_status(mam::OrderStatus_TotalMatch);
            vec[index].set_errcode(mam::JErrorType_TooHighFrequency);
        }

        auto e1 = std::chrono::steady_clock::now();
        std::cout << "缺省尺寸耗时(毫秒): " << timediff<std::chrono::milliseconds>(b1, e1) << std::endl;
    }

    // 大页尺寸
    {
        std::vector<Type, hpallocator<Type>> vec;
        vec.reserve(sz * sizeof(Type));

        for (size_t i = 0; i < sz; i++) {
            vec.emplace_back(Type());
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));    // 让 CPU 放松一会儿
        auto b1 = std::chrono::steady_clock::now();

        for (int j = 0; j < total; j++) {
            // int index = rd.given();
            int index = ((sz / total) * j * 97) % sz;
            vec[index].set_reqid(j);
            vec[index].set_symbol(std::to_string(j));
            vec[index].set_orderprice(j * 1000);
            vec[index].set_ordervolume(j * 100);
            vec[index].set_force(j % 2);
            vec[index].set_createtime(j * 1000);
            vec[index].set_closetime(j * 10000 + j * 100);
            vec[index].set_status(mam::OrderStatus_TotalMatch);
            vec[index].set_errcode(mam::JErrorType_TooHighFrequency);
        }

        auto e1 = std::chrono::steady_clock::now();
        std::cout << "大页尺寸耗时(毫秒): " << timediff<std::chrono::milliseconds>(b1, e1) << std::endl;
    }

    // 使用指针
    // 测试发现使用指针的效率与上述两者相当，难道真的如此吗？
    // 在这里的测试中，分配动态内存是在一个 for 循环中连续进行的，分配场景单一，其分配的内存很可能是局部连续的。所以 cache 命中很高。
    {
        std::vector<Ptr> vec;
        vec.reserve(sz * sizeof(Ptr));

        for (size_t i = 0; i < sz; i++) {
            vec.emplace_back(new Type());
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));    // 让 CPU 放松一会儿
        auto b1 = std::chrono::steady_clock::now();

        for (int j = 0; j < total; j++) {
            // int index = rd.given();
            int index = ((sz / total) * j * 97) % sz;
            vec[index]->set_reqid(j);
            vec[index]->set_symbol(std::to_string(j));
            vec[index]->set_orderprice(j * 1000);
            vec[index]->set_ordervolume(j * 100);
            vec[index]->set_force(j % 2);
            vec[index]->set_createtime(j * 1000);
            vec[index]->set_closetime(j * 10000 + j * 100);
            vec[index]->set_status(mam::OrderStatus_TotalMatch);
            vec[index]->set_errcode(mam::JErrorType_TooHighFrequency);
        }

        auto e1 = std::chrono::steady_clock::now();
        std::cout << "使用指针耗时(毫秒): " << timediff<std::chrono::milliseconds>(b1, e1) << std::endl;
    }

    std::cout << "=== over ===" << std::endl;
    
    getchar();

    return 0;
}