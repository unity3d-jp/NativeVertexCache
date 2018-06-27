#include "Plugin/PrecompiledHeader.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/OutputGeomCache.h"
#include "Plugin/GeomCache.h"
#include "Plugin/nvcAPI.h"

namespace nvc {

// std::copy can be extremely slow on some platform/compilers. this is a faster altenative.
template<class T>
static inline void copy(const std::vector<T>& src, T *dst)
{
    std::memcpy(dst, src.data(), sizeof(T) * src.size());
}

} // namespace nvc


nvcAPI nvc::InputGeomCache* nvcIGCCreate(const nvc::GeomCacheDesc *descs)
{
    return new nvc::InputGeomCache(descs);
}
nvcAPI void nvcIGCRelease(nvc::InputGeomCache *self)
{
    delete self;
}
nvcAPI void nvcIGCAddData(nvc::InputGeomCache * self, float time, const nvc::GeomCacheData * data)
{
    if (self) {
        self->addData(time, data);
    }
}

nvcAPI nvc::OutputGeomCache* nvcOGCCreate()
{
    return new nvc::OutputGeomCache();
}

nvcAPI void nvcOGCRelease(nvc::OutputGeomCache *self)
{
    delete self;
}

nvcAPI int nvcOGCGetVertexCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->points.size() : 0;
}

nvcAPI int nvcOGCGetIndexCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->indices.size() : 0;
}

nvcAPI void nvcOGCGetIndices(nvc::OutputGeomCache *self, int *dst)
{
    if (self) {
        nvc::copy(self->indices, dst);
    }
}

nvcAPI void nvcOGCGetPoints(nvc::OutputGeomCache *self, nvc::float3 *dst)
{
    if (self) {
        nvc::copy(self->points, dst);
    }
}

nvcAPI void nvcOGCGetNormals(nvc::OutputGeomCache *self, nvc::float3 *dst)
{
    if (self) {
        nvc::copy(self->normals, dst);
    }
}

nvcAPI void nvcOGCGetTangents(nvc::OutputGeomCache *self, nvc::float4 *dst)
{
    if (self) {
        nvc::copy(self->tangents, dst);
    }
}

nvcAPI void nvcOGCGetUVs(nvc::OutputGeomCache *self, nvc::float2 *dst)
{
    if (self) {
        nvc::copy(self->uvs, dst);
    }
}

nvcAPI void nvcOGCGetColors(nvc::OutputGeomCache *self, nvc::float4 *dst)
{
    if (self) {
        nvc::copy(self->colors, dst);
    }
}


nvcAPI nvc::GeomCache* nvcGCCreate()
{
    return new nvc::GeomCache();
}

nvcAPI void nvcGCRelease(nvc::GeomCache *self)
{
    delete self;
}

nvcAPI int nvcGCOpen(nvc::GeomCache *self, const char *path)
{
    if (self) {
        return self->open(path);
    }
    return false;
}

nvcAPI void nvcGCClose(nvc::GeomCache *self)
{
    if (self) {
        self->close();
    }
}

nvcAPI void nvcGCSetCurrentTime(nvc::GeomCache *self, float time)
{
    if (self) {
        self->setCurrentFrame(time);
    }
}

nvcAPI void nvcGCGetCurrentCache(nvc::GeomCache *self, nvc::OutputGeomCache *ogc)
{
    if (self && ogc) {
        self->assignCurrentDataToMesh(*ogc);
    }
}
