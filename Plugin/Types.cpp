#include "Plugin/PrecompiledHeader.h"
#include "Types.h"

namespace nvc {

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
