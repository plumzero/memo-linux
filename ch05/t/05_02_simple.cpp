
#include "hpallocator.h"
#include <vector>
#include <algorithm>
#include <iostream>

template<typename _Tp>
struct PrintElements
{
    void operator()(const _Tp& t) const {
        std::cout << t << " ";
    }
};

// 还需要做的测试
// 1. 内存反复分配/释放
// 2. 复杂结构体的测试
// 3. 所有方法的测试

int main()
{
    std::vector<int64_t, hpallocator<int64_t>> vec;
    vec.reserve(2);

    getchar();

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    getchar();

    vec.pop_back();

    std::for_each(vec.begin(), vec.end(), PrintElements<int64_t>());

    return 0;
}