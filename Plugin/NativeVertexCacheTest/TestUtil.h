#include "Plugin/Pcg.h"
#include <chrono>

template<class T>
inline bool NearEqual(T a, T b, T eps)
{
    return std::abs(a - b) <= std::abs(eps);
}

inline void ThrowError(const char *format, ...)
{
    char buf[1024];

    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    throw std::runtime_error(buf);
}

template<typename T>
inline void FillRandom(T& vec, uint64_t state = 0x900dbeef, uint64_t inc = 0x87654321) {
    using ValueType = T::value_type;
    Pcg pcg { state, inc };
    for(auto& v : vec) {
        v = static_cast<ValueType>(pcg.getUint32());
    }
}

template<typename T>
static void FillSequence(T& vec) {
    using ValueType = T::value_type;
    auto x = static_cast<ValueType>(0);
    for(auto& v : vec) {
        v = x;
        ++x;
    }
}

// usage:
//    const auto start_time = GetHighResolutionClock();
//    ... do something ...
//    const auto end_time = GetHighResolutionClock();
//    const double diff_time_in_seconds = GetSeconds(start_time, end_time);
//    printf("%f seconds\n", diff_time_in_seconds);
//
inline std::chrono::high_resolution_clock::time_point GetHighResolutionClock() {
    return std::chrono::high_resolution_clock::now();
}

inline double GetSeconds(std::chrono::high_resolution_clock::time_point begin, std::chrono::high_resolution_clock::time_point end) {
    const auto d = static_cast<std::chrono::duration<double>>(end - begin);
    return d.count();
}
