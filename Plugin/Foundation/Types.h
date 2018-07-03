#pragma once
#include <cstdint>

namespace nvc {

using time_t = float;


// note: this half doesn't care about Inf nor NaN. simply round down minor bits of exponent and mantissa.
struct half
{
    uint16_t data;

    half() : data(0) {}
    half(const half& v) : data(v.data) {}

    half(float v)
    {
        uint32_t n = (uint32_t&)v;
        uint16_t sign_bit = (n >> 16) & 0x8000;
        uint16_t exponent = (((n >> 23) - 127 + 15) & 0x1f) << 10;
        uint16_t mantissa = (n >> (23 - 10)) & 0x3ff;

        data = sign_bit | exponent | mantissa;
    }

    half& operator=(float v)
    {
        *this = half(v);
        return *this;
    }

    float to_float() const
    {
        uint32_t sign_bit = (data & 0x8000) << 16;
        uint32_t exponent = ((((data >> 10) & 0x1f) - 15 + 127) & 0xff) << 23;
        uint32_t mantissa = (data & 0x3ff) << (23 - 10);

        uint32_t r = sign_bit | exponent | mantissa;
        return (float&)r;
    }
};

struct snorm16
{
    int16_t data;

    snorm16() : data(0) {}
    snorm16(const snorm16& v) : data(v.data) {}
    snorm16(float v) : data(int16_t(v * 32767.0f)) {}

    snorm16& operator=(float v)
    {
        *this = snorm16(v);
        return *this;
    }
    float to_float() const { return (float)data / 32767.0f; }

    static snorm16 zero() { return snorm16(0.0f); }
    static snorm16 one() { return snorm16(1.0f); }
};

struct unorm16
{
    uint16_t data;

    unorm16() : data(0) {}
    unorm16(const unorm16& v) : data(v.data) {}
    unorm16(float v) : data(uint16_t(v * 65535.0f)) {}

    unorm16 & operator=(float v)
    {
        *this = unorm16(v);
        return *this;
    }
    float to_float() const { return (float)data / 65535.0f; }

    static unorm16 zero() { return unorm16(0.0f); }
    static unorm16 one() { return unorm16(1.0f); }
};



template<class T>
struct tvec2
{
    using value_type = T;
    static const int vector_size = 2;

    T data[2];

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }
};

template<class T>
struct tvec3
{
    using value_type = T;
    static const int vector_size = 3;

    T data[3];

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }
};

template<class T>
struct tvec4
{
    using value_type = T;
    static const int vector_size = 4;

    T data[4];

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }
};

using float2 = tvec2<float>;
using float3 = tvec3<float>;
using float4 = tvec4<float>;

using half2 = tvec2<half>;
using half3 = tvec3<half>;
using half4 = tvec4<half>;

using snorm16x2 = tvec2<snorm16>;
using snorm16x3 = tvec3<snorm16>;
using snorm16x4 = tvec4<snorm16>;

using unorm16x2 = tvec2<unorm16>;
using unorm16x3 = tvec3<unorm16>;
using unorm16x4 = tvec4<unorm16>;


float  to_float(float  v); // 
float2 to_float(float2 v); // 
float3 to_float(float3 v); // 
float4 to_float(float4 v); // just return v. convenient for template functions
float  to_float(half  v);
float2 to_float(half2 v);
float3 to_float(half3 v);
float4 to_float(half4 v);
float  to_float(snorm16   v);
float2 to_float(snorm16x2 v);
float3 to_float(snorm16x3 v);
float4 to_float(snorm16x4 v);
float  to_float(unorm16   v);
float2 to_float(unorm16x2 v);
float3 to_float(unorm16x3 v);
float4 to_float(unorm16x4 v);

} // namespace nvc

