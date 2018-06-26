#include "Plugin/PrecompiledHeader.h"
#include "Types.h"

namespace nvc {

half::half(float v)
{
    uint32_t n = (uint32_t&)v;
    uint16_t sign_bit = (n >> 16) & 0x8000;
    uint16_t exponent = (((n >> 23) - 127 + 15) & 0x1f) << 10;
    uint16_t mantissa = (n >> (23 - 10)) & 0x3ff;

    data = sign_bit | exponent | mantissa;
}

half & half::operator=(float v)
{
    *this = half(v);
    return *this;
}

float half::to_float() const
{
    uint32_t sign_bit = (data & 0x8000) << 16;
    uint32_t exponent = ((((data >> 10) & 0x1f) - 15 + 127) & 0xff) << 23;
    uint32_t mantissa = (data & 0x3ff) << (23 - 10);

    uint32_t r = sign_bit | exponent | mantissa;
    return (float&)r;
}

snorm16 & snorm16::operator=(float v)
{
    *this = snorm16(v);
    return *this;
}

unorm16 & unorm16::operator=(float v)
{
    *this = unorm16(v);
    return *this;
}

float  to_float(float  v) { return v; }
float2 to_float(float2 v) { return v; }
float3 to_float(float3 v) { return v; }
float4 to_float(float4 v) { return v; }
float  to_float(half  v) { return v.to_float(); }
float2 to_float(half2 v) { return { v[0].to_float(), v[1].to_float() }; }
float3 to_float(half3 v) { return { v[0].to_float(), v[1].to_float(), v[2].to_float() }; }
float4 to_float(half4 v) { return { v[0].to_float(), v[1].to_float(), v[2].to_float(), v[3].to_float() }; }
float  to_float(snorm16   v) { return v.to_float(); }
float2 to_float(snorm16x2 v) { return { v[0].to_float(), v[1].to_float() }; }
float3 to_float(snorm16x3 v) { return { v[0].to_float(), v[1].to_float(), v[2].to_float() }; }
float4 to_float(snorm16x4 v) { return { v[0].to_float(), v[1].to_float(), v[2].to_float(), v[3].to_float() }; }
float  to_float(unorm16   v) { return v.to_float(); }
float2 to_float(unorm16x2 v) { return { v[0].to_float(), v[1].to_float() }; }
float3 to_float(unorm16x3 v) { return { v[0].to_float(), v[1].to_float(), v[2].to_float() }; }
float4 to_float(unorm16x4 v) { return { v[0].to_float(), v[1].to_float(), v[2].to_float(), v[3].to_float() }; }

} // namespace nvc
