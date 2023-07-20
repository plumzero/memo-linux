
#include <stdio.h>
#include <stdint.h>
#include <chrono>
#include <thread>
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

#define CACHELINE_SIZE 64
#define CACHE_PADDING(N) char N##N[CACHELINE_SIZE - sizeof(N)]

namespace nocl {
struct Header
{
  uint8_t     ver[3];
  uint8_t     orthocode;
  uint16_t    bodysize;
  uint16_t    msgid;
  uint64_t    origin;
  uint64_t    orient;
};

Header header;
}

namespace cl {
struct Header
{
  uint8_t     ver[3];
  CACHE_PADDING(ver);
  uint8_t     orthocode;
  CACHE_PADDING(orthocode);
  uint16_t    bodysize;
  CACHE_PADDING(bodysize);
  uint16_t    msgid;
  CACHE_PADDING(msgid);
  uint64_t    origin;
  CACHE_PADDING(origin);
  uint64_t    orient;
  CACHE_PADDING(orient);
};

Header header;
}

int main()
{
  printf("sizeof(nocl::Header)=%lu\n", sizeof(nocl::Header));
  printf("sizeof(cl::Header)=%lu\n", sizeof(cl::Header));

  const int count = 1000000000;
  {
    auto b = std::chrono::steady_clock::now();
    std::thread th1 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        nocl::header.orthocode = i;
      }
    });
    std::thread th2 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        nocl::header.bodysize = i;
      }
    });
    th1.join();
    th2.join();
    auto e = std::chrono::steady_clock::now();
    printf("缓存行不填充耗时: %ld\n", timediff<std::chrono::microseconds>(b, e));
  }

  {
    auto b = std::chrono::steady_clock::now();
    std::thread th1 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        nocl::header.orthocode = i;
      }
    });
    std::thread th2 = std::thread([](){
      for (size_t i = 0; i < count; i++) {
        nocl::header.bodysize = i;
      }
    });
    th1.join();
    th2.join();
    auto e = std::chrono::steady_clock::now();
    printf("缓存行填充耗时: %ld\n", timediff<std::chrono::microseconds>(b, e));
  }

  return 0;
}