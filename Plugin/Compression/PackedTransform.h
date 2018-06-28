#pragma once

#include "Plugin/Types.h"

#include <algorithm>
#include <cmath>

namespace nvc
{

struct AABB
{
	float3 min {};
	float3 extents {};

	AABB()
	{
		min[0] = min[1] = min[2] = 0.0f;
		extents[0] = extents[1] = extents[2] = 0.0f;
	}

	AABB(const float3& m)
	{
		min = m;
		extents[0] = extents[1] = extents[2] = 0.0f;
	}

	AABB(const float3& m, const float3& e)
	{
		min = m;
		extents = e;
	}

	static AABB Build(const float3 *points, size_t count)
	{
		float3 min = points[0];
		float3 max = points[0];
		for (size_t i = 1; i < count; ++i)
		{
			min[0] = std::min(min[0], points[i][0]);
			min[1] = std::min(min[1], points[i][1]);
			min[2] = std::min(min[2], points[i][2]);

			max[0] = std::max(max[0], points[i][0]);
			max[1] = std::max(max[1], points[i][1]);
			max[2] = std::max(max[2], points[i][2]);
		}

		float3 extents;
		extents[0] = max[0] - min[0];
		extents[1] = max[1] - min[1];
		extents[2] = max[2] - min[2];

		return AABB(min, extents);
	}
};

inline unorm16x3 PackPoint(const AABB& aabb, const float3& point)
{
	unorm16x3 packed;
	packed[0] = (point[0] - aabb.min[0]) / aabb.extents[0];
	packed[1] = (point[1] - aabb.min[1]) / aabb.extents[1];
	packed[2] = (point[2] - aabb.min[2]) / aabb.extents[2];
	return packed;
}

inline float3 UnpackPoint(const AABB& aabb, const unorm16x3& point)
{
	float3 unpacked;
	unpacked[0] = aabb.min[0] + (point[0].to_float() * aabb.extents[0]);
	unpacked[1] = aabb.min[1] + (point[1].to_float() * aabb.extents[1]);
	unpacked[2] = aabb.min[2] + (point[2].to_float() * aabb.extents[2]);
	return unpacked;
}



static constexpr float Squared(float a) { return a * a; }

#define QUATERNION_BITS_PER_COMPONENT	(10)

struct PackedQuaternion
{
	static constexpr float SQURTF_2 = 1.41421356237f;
	static constexpr float NORM_FACTOR = 1.0f / SQURTF_2;

	static constexpr uint32_t PackFloatNormalize(float a, float norm, int bits)
	{
		return static_cast<uint32_t>(((a * norm + 1.0f) * 0.5f) * (0x1 << bits));
	}

	static constexpr float UnpackFloatNormalized(uint32_t a, float norm, int bits)
	{
		return ((a * (1.0f / (0x1 << bits))) * 2.0f - 1.0f) * norm;
	}

	union
	{
		struct
		{
			uint32_t X0 : QUATERNION_BITS_PER_COMPONENT;
			uint32_t X1 : QUATERNION_BITS_PER_COMPONENT;
			uint32_t X2 : QUATERNION_BITS_PER_COMPONENT;
			uint32_t drop : 2;
		};
		uint32_t Packed;
	};

	PackedQuaternion()
	{
		// Initialize to identity.
		float4 identity; identity[0] = identity[1] = identity[2] = 0.0f; identity[3] = 1.0f;

		const PackedQuaternion packedIdentity(identity);
		Packed = packedIdentity.Packed;
	}

	PackedQuaternion(const float4& quat)
	{
		// Start: x² + y² + z² + w² = 1
		// Idea: we can get any component like -> w = 1 - sqrt(x² + y² + z²)
		// Algo:
		//	dropmax(x, y, z, w) => (x0, x1, x2) (k)
		//	if (k < 0) 
		//		Flip values: <x0, x1, x2> = <-x0, -x1, -x2>  (Since <x, y, z, w> = <-x, -y, -z, -w>)
		//	Normalize().
		//	PackFloat().

		float a0, a1, a2;

		drop = dropmax(Squared(quat[0]), Squared(quat[1]), Squared(quat[2]), Squared(quat[3]));
		if (drop == 0)
		{
			const float flip = quat[0] < 0.0f ? -1.0f : 1.0f;
			a0 = quat[1] * flip;
			a1 = quat[2] * flip;
			a2 = quat[3] * flip;
		}
		else if (drop == 1)
		{
			const float flip = quat[1] < 0.0f ? -1.0f : 1.0f;
			a0 = quat[0] * flip;
			a1 = quat[2] * flip;
			a2 = quat[3] * flip;
		}
		else if (drop == 2)
		{
			const float flip = quat[2] < 0.0f ? -1.0f : 1.0f;
			a0 = quat[0] * flip;
			a1 = quat[1] * flip;
			a2 = quat[3] * flip;
		}
		else
		{
			const float flip = quat[3] < 0.0f ? -1.0f : 1.0f;
			a0 = quat[0] * flip;
			a1 = quat[1] * flip;
			a2 = quat[2] * flip;
		}

		// Normalize and pack.
		X0 = PackFloatNormalize(a0, SQURTF_2, QUATERNION_BITS_PER_COMPONENT);
		X1 = PackFloatNormalize(a1, SQURTF_2, QUATERNION_BITS_PER_COMPONENT);
		X2 = PackFloatNormalize(a2, SQURTF_2, QUATERNION_BITS_PER_COMPONENT);
	}

	operator float4() const
	{
		const float a0 = UnpackFloatNormalized(X0, NORM_FACTOR, QUATERNION_BITS_PER_COMPONENT);
		const float a1 = UnpackFloatNormalized(X0, NORM_FACTOR, QUATERNION_BITS_PER_COMPONENT);
		const float a2 = UnpackFloatNormalized(X0, NORM_FACTOR, QUATERNION_BITS_PER_COMPONENT);
		
		float4 quat;
		if (drop == 0)
		{
			quat[0] = 1 - sqrtf(Squared(a0) + Squared(a1)+ Squared(a2));
			quat[1] = a0;
			quat[2] = a1;
			quat[3] = a2;
		}
		else if (drop == 1)
		{
			quat[0] = a0;
			quat[1] = 1 - sqrtf(Squared(a0) + Squared(a1) + Squared(a2));
			quat[2] = a1;
			quat[3] = a2;
		}
		else if (drop == 2)
		{
			quat[0] = a0;
			quat[1] = a1;
			quat[2] = 1 - sqrtf(Squared(a0) + Squared(a1) + Squared(a2));
			quat[3] = a2;
		}
		else
		{
			quat[0] = a0;
			quat[1] = a1;
			quat[2] = a2;
			quat[3] = 1 - sqrtf(Squared(a0) + Squared(a1) + Squared(a2));
		}

		return quat;
	}

	template <typename TType>
	static int dropmax(TType a, TType b, TType c, TType d)
	{
		if (a > b && a > c && a > d) return 0;
		if (b > a && b > c && c > d) return 1;
		if (c > a && c > b && c > d) return 2;
		return 3;
	}
};

// https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
inline float2 OctEncode(float3 n)
{
	//n /= (abs(n.x) + abs(n.y) + abs(n.z));
	//n.xy = n.z >= 0.0 ? n.xy : OctWrap(n.xy);
	//n.xy = n.xy * 0.5 + 0.5;
	//return n.xy;

	const float k = 1.0f / (abs(n[0]) + abs(n[1]) + abs(n[2]));
	n[0] = n[0] * k;
	n[1] = n[1] * k;
	n[2] = n[2] * k;

	n[0] = n[2] >= 0.0f ? n[0] : (1.0f - abs(n[1])) * (n[0] >= 0.0 ? 1.0f : -1.0f);
	n[1] = n[2] >= 0.0f ? n[1] : (1.0f - abs(n[0])) * (n[1] >= 0.0 ? 1.0f : -1.0f);
	
	float2 oct;
	oct[0] = n[0] * 0.5f + 0.5f;
	oct[1] = n[1] * 0.5f + 0.5f;

	return oct;
}

inline float2 OctEncode(float4 n)
{
	return OctEncode(*reinterpret_cast<float3*>(&n));
}

inline float3 OctDecode(float2 f)
{
	//// https://twitter.com/Stubbesaurus/status/937994790553227264
	//float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
	//float t = saturate(-n.z);
	//n.xy += n.xy >= 0.0 ? -t : t;
	//return normalize(n);

	f[0] = f[0] * 2.0f - 1.0f;
	f[1] = f[1] * 2.0f - 1.0f;

	float3 n; n[0] = f[0]; n[1] = f[1]; n[2] = 1.0f - abs(f[0]) - abs(f[1]);
	const float t = std::max(std::min(-n[2], 1.0f), 0.0f);
	n[0] += n[0] >= 0.0f ? -t : t;
	n[1] += n[1] >= 0.0f ? -t : t;

	float k = sqrtf(Squared(n[0]) + Squared(n[1]) + Squared(n[2]));
	n[0] = n[0] / k;
	n[1] = n[1] / k;
	n[2] = n[2] / k;

	return n;
}

struct PackedTransform
{
	unorm16x3 Translation;
	//uint16_t Padding{};
	unorm16x2 Normal;
	unorm16x2 Tangent;
};

} // namespace nvc
