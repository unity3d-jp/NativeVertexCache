#pragma once

#ifdef _WIN32
    #ifdef NVC_IMPL
        #define nvcAPI extern "C" __declspec(dllexport)
    #else
        #define nvcAPI extern "C" __declspec(dllimport)
    #endif
#else
    #define nvcAPI extern "C"
#endif

#include "./Types.h"
#include "./DataFormat.h"

namespace nvc {
class InputGeomCache;
class OutputGeomCache;
class GeomCache;
} // namespace nvc

nvcAPI nvc::InputGeomCache* nvcIGCCreate(const char *path, const nvc::GeomCacheDesc *descs);
nvcAPI void nvcIGCRelease(nvc::InputGeomCache *self);
nvcAPI void nvcIGCAddData(nvc::InputGeomCache *self, float time, const nvc::GeomCacheData *data);

nvcAPI int  nvcOGCGetVertexCount(nvc::OutputGeomCache *self);
nvcAPI int  nvcOGCGetIndexCount(nvc::OutputGeomCache *self);
nvcAPI void nvcOGCGetIndices(nvc::OutputGeomCache *self, int *dst);
nvcAPI void nvcOGCGetPoints(nvc::OutputGeomCache *self, nvc::float3 *dst);
nvcAPI void nvcOGCGetNormals(nvc::OutputGeomCache *self, nvc::float3 *dst);
nvcAPI void nvcOGCGetTangents(nvc::OutputGeomCache *self, nvc::float4 *dst);
nvcAPI void nvcOGCGetUVs(nvc::OutputGeomCache *self, nvc::float2 *dst);
nvcAPI void nvcOGCGetColors(nvc::OutputGeomCache *self, nvc::float4 *dst);
