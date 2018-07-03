#pragma once
#include "Plugin/Foundation/Types.h"
#include "Plugin/Foundation/RawVector.h"
#include "Plugin/GeomCacheData.h"

namespace nvc {

// vertex arrays holder to update Mesh
// this struct will be removed when integrate this plugin into Unity engine (in that case we can update Mesh directly)
class OutputGeomCache
{
public:
    RawVector<GeomMesh> meshes;
    RawVector<GeomSubmesh> submeshes;
    RawVector<int> indices;
    RawVector<float3> points;
    RawVector<float3> normals;
    RawVector<float4> tangents;
    RawVector<float2> uv0, uv1;
    RawVector<float4> colors;

    bool copyIndices(const GeomSubmesh &subm, int *dst);
    bool copyPoints(const GeomMesh &mesh, float3 *dst);
    bool copyNormals(const GeomMesh &mesh, float3 *dst);
    bool copyTangents(const GeomMesh &mesh, float4 *dst);
    bool copyUV0(const GeomMesh &mesh, float2 *dst);
    bool copyUV1(const GeomMesh &mesh, float2 *dst);
    bool copyColors(const GeomMesh &mesh, float4 *dst);
};

} // namespace nvc
