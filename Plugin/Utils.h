#pragma once

namespace nvc {

// std::copy can be extremely slow on some platform/compilers. these faster altenative.
template<class T>
static inline void copy(const T* src, T *dst, size_t count)
{
    std::memcpy(dst, src, sizeof(T) * count);
}

} // namespace nvc
