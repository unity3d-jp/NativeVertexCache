#pragma once

namespace nvc {

using time_t = float;


struct half
{
    uint16_t data;

    half(const half& v) : data(v.data) {}
    half(float v);
    float to_float() const;
};

struct snorm16
{
    int16_t data;

    snorm16() : data(0) {}
    snorm16(const snorm16& v) : data(v.data) {}
    snorm16(float v) : data(uint16_t(v * 32767.0f)) {}
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
    float to_float() const { return (float)data / 65535.0f; }

    static unorm16 zero() { return unorm16(0.0f); }
    static unorm16 one() { return unorm16(1.0f); }
};



template<class T>
struct tvec2
{
    T data[2];

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }
};

template<class T>
struct tvec3
{
    T data[3];

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }
};

template<class T>
struct tvec4
{
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

} // namespace nvc

