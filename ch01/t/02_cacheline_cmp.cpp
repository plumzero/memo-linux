
#include <stdio.h>
#include <thread>
#include <chrono>
#include <type_traits>

template <typename TU,
          class = typename std::enable_if<std::is_same<TU, std::chrono::hours>::value ||
                                          std::is_same<TU, std::chrono::minutes>::value ||
                                          std::is_same<TU, std::chrono::seconds>::value ||
                                          std::is_same<TU, std::chrono::milliseconds>::value ||
                                          std::is_same<TU, std::chrono::microseconds>::value ||
                                          std::is_same<TU, std::chrono::nanoseconds>::value>::type,
          typename TP,
          class = typename std::enable_if<std::is_same<TP, std::chrono::time_point<std::chrono::system_clock>>::value ||
                                          std::is_same<TP, std::chrono::time_point<std::chrono::steady_clock>>::value>::type
         >
inline long timediff(TP t1, TP t2)
{
    return (long)std::chrono::duration_cast<TU>(t2 - t1).count();
}

// no cacheline padding
// 定义一个长度为 2 的数组，保证两个变量在同一缓存行中，以模拟伪共享问题。
// 然后在两个线程中分别修改这个数组中的不同元素，测试耗时
namespace no_cacheline_padding {
class Entity {
public:
  volatile long x = 1L;
};

Entity* arr = new Entity[2];
}

// cacheline padding
// 定义一个包含 7 个 long 类型的"无实际意义"字段的填充对象
// 实际对象 Entity 继承填充对象，达到 7+1=8 个 long 类型字段，可以填充一整个 64 字节的缓存行
namespace cacheline_padding {
class Padding {
protected:
  volatile long p1, p2, p3, p4, p5, p6, p7;
};
class Entity : public Padding {
public:
  volatile long x = 1L;
};

Entity* arr = new Entity[2];
}

int main()
{
  const int count = 1000000000;
  {
    printf("sizeof(no_cacheline_padding::Entity)=%lu\n", sizeof(no_cacheline_padding::Entity));
    auto b = std::chrono::steady_clock::now();
    std::thread th1 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        no_cacheline_padding::arr[0].x = i;
      }
    });
    std::thread th2 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        no_cacheline_padding::arr[1].x = i;
      }
    });
    th1.join();
    th2.join();
    auto e = std::chrono::steady_clock::now();
    printf("缓存行不填充耗时: %ld\n", timediff<std::chrono::microseconds>(b, e));
  }

  {
    printf("sizeof(cacheline_padding::Entity)=%lu\n", sizeof(cacheline_padding::Entity));
    auto b = std::chrono::steady_clock::now();
    std::thread th1 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        cacheline_padding::arr[0].x = i;
      }
    });
    std::thread th2 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        cacheline_padding::arr[1].x = i;
      }
    });
    th1.join();
    th2.join();
    auto e = std::chrono::steady_clock::now();
    printf("缓存行填充耗时: %ld\n", timediff<std::chrono::microseconds>(b, e));
  }

  return 0;
}