#pragma once

#ifdef _WIN32
    #define NVC_ENABLE_PPL
#endif

#include <atomic>
#if defined(NVC_ENABLE_PPL)
    #include <ppl.h>
#elif defined(NVC_ENABLE_TBB)
    #include <tbb/tbb.h>
#endif

namespace nvc {

template<class Index, class Body>
inline void parallel_for(Index begin, Index end, const Body& body)
{
#if defined(NVC_ENABLE_PPL)
    concurrency::parallel_for(begin, end, body);
#elif defined(NVC_ENABLE_TBB)
    tbb::parallel_for(begin, end, body);
#else
    for (; begin != end; ++begin) { body(begin); }
#endif
}

#if defined(NVC_ENABLE_PPL) || defined(NVC_ENABLE_TBB)
template<class Body>
inline void parallel_for(int begin, int end, int granularity, const Body& body)
{
    int num_elements = end - begin;
    int num_blocks = ceildiv(num_elements, granularity);
    parallel_for(0, num_blocks, [&](int i) {
        int begin = granularity * i;
        int end = std::min<int>(granularity * (i + 1), num_elements);
        for (; begin != end; ++begin) {
            body(begin);
        }
    });
}
template<class Body>
inline void parallel_for_blocked(int begin, int end, int granularity, const Body& body)
{
    int num_elements = end - begin;
    int num_blocks = ceildiv(num_elements, granularity);
    parallel_for(0, num_blocks, [&](int i) {
        int begin = granularity * i;
        int end = std::min<int>(granularity * (i + 1), num_elements);
        body(begin, end);
    });
}
#else
template<class Body>
inline void parallel_for(int begin, int end, int /*granularity*/, const Body& body)
{
    for (; begin != end; ++begin) { body(begin); }
}
template<class Body>
inline void parallel_for_blocked(int begin, int end, int /*granularity*/, const Body& body)
{
    for (; begin != end; ++begin) { body(begin); }
}
#endif

template<class Iter, class Body>
inline void parallel_for_each(Iter begin, Iter end, const Body& body)
{
#if defined(NVC_ENABLE_PPL)
    concurrency::parallel_for_each(begin, end, body);
#elif defined(NVC_ENABLE_TBB)
    tbb::parallel_for_each(begin, end, body);
#else
    for (; begin != end; ++begin) { body(*begin); }
#endif
}


#if defined(NVC_ENABLE_PPL)

template <class... Bodies>
inline void parallel_invoke(Bodies... bodies) { concurrency::parallel_invoke(bodies...); }

#elif defined(NVC_ENABLE_TBB)

template <class... Bodies>
inline void parallel_invoke(Bodies... bodies) { tbb::parallel_invoke(bodies...); }

#else

template <class Body>
inline void parallel_invoke(const Body& body) { body(); }

template<typename Body, typename... Args>
void parallel_invoke(const Body& first, Args... args) {
    first();
    parallel_invoke(args...);
}

#endif

template<class T>
class scoped_lock
{
public:
    scoped_lock(T& mutex) : m_mutex(mutex)
    {
        mutex.lock();
    }
    ~scoped_lock()
    {
        m_mutex.unlock();
    }

public:
    T& m_mutex;
};

class spin_mutex
{
public:
    using lock_t = scoped_lock<spin_mutex>;

    void lock()
    {
        while (m_flag.test_and_set(std::memory_order_acquire)) {}
    }

    void unlock()
    {
        m_flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

} // namespace nvc

