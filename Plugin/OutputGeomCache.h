#pragma once
#include "Plugin/Types.h"
#include "Plugin/GeomCacheData.h"

namespace nvc {

// vertex arrays holder to update Mesh
// this struct will be removed when integrate this plugin into Unity engine (in that case we can update Mesh directly)
class OutputGeomCache
{
public:
    std::vector<int> indices;
    std::vector<float3> points;
    std::vector<float3> normals;
    std::vector<float4> tangents;
    std::vector<float2> uv0, uv1;
    std::vector<float4> colors;

    void copyIndices(const GeomSubmesh &subm, int *dst);
    void copyPoints(const GeomMesh &mesh, float3 *dst);
    void copyNormals(const GeomMesh &mesh, float3 *dst);
    void copyTangents(const GeomMesh &mesh, float4 *dst);
    void copyUV0(const GeomMesh &mesh, float2 *dst);
    void copyUV1(const GeomMesh &mesh, float2 *dst);
    void copyColors(const GeomMesh &mesh, float4 *dst);
};

} // namespace nvc
