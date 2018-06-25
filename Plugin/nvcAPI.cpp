#include "Plugin/PrecompiledHeader.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/OutputGeomCache.h"
#include "Plugin/nvcAPI.h"

namespace nvc {

// std::copy can be extremely slow in some platform/compiler. this is faster altenative.
template<class T>
static inline void copy(const std::vector<T>& src, T *dst)
{
    std::memcpy(dst, src.data(), sizeof(T) * src.size());
}

} // namespace nvc


nvcAPI int nvcGetVertexCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->points.size() : 0;
}

nvcAPI int nvcGetIndexCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->indices.size() : 0;
}

nvcAPI void nvcFillIndices(nvc::OutputGeomCache *self, int *dst)
{
    if (self) {
        nvc::copy(self->indices, dst);
    }
}

nvcAPI void nvcFillPoints(nvc::OutputGeomCache *self, nvc::float3 *dst)
{
    if (self) {
        nvc::copy(self->points, dst);
    }
}

nvcAPI void nvcFillNormals(nvc::OutputGeomCache *self, nvc::float3 *dst)
{
    if (self) {
        nvc::copy(self->normals, dst);
    }
}

nvcAPI void nvcFillTangents(nvc::OutputGeomCache *self, nvc::float4 *dst)
{
    if (self) {
        nvc::copy(self->tangents, dst);
    }
}

nvcAPI void nvcFillUVs(nvc::OutputGeomCache *self, nvc::float2 *dst)
{
    if (self) {
        nvc::copy(self->uvs, dst);
    }
}

nvcAPI void nvcFillColors(nvc::OutputGeomCache *self, nvc::float4 *dst)
{
    if (self) {
        nvc::copy(self->colors, dst);
    }
}