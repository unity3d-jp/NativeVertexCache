#pragma once
#include "Plugin/Types.h"

namespace nvc {

// vertex arrays holder to update Mesh
// this struct will be removed when integrate this plugin into Unity engine (in that case we can update Mesh directly)
struct OutputGeomCache
{
    std::vector<int> indices;
    std::vector<float3> points;
    std::vector<float3> normals;
    std::vector<float4> tangents;
    std::vector<float2> uvs;
    std::vector<float4> colors;
};

} // namespace nvc
