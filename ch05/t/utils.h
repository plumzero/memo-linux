
#include <chrono>
#include <type_traits>
#include <random>

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

static std::default_random_engine _rng { std::random_device()() };

class randev
{
    using Range = std::uniform_int_distribution<int>::param_type;

public:
    randev(int rb, int re) {
        if (rb > re) throw(1);
        _dist.param(Range{ rb, re });
        // std::cout << "Range from " << _dist.a() << " to " << _dist.b() << std::endl;
    }
    ~randev() {}

    int given() {
        return _dist(_rng);
    }

private:
    std::uniform_int_distribution<int> _dist;
};
