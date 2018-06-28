#pragma once

#define nvcPluginVersion 20180628

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
#include "./GeomCacheData.h"

namespace nvc {
class InputGeomCache;
class OutputGeomCache;
class GeomCache;
} // namespace nvc

nvcAPI nvc::InputGeomCache* nvcIGCCreate(const nvc::GeomCacheDesc *descs);
nvcAPI void nvcIGCRelease(nvc::InputGeomCache *self);
nvcAPI void nvcIGCAddData(nvc::InputGeomCache *self, float time, const nvc::GeomCacheData *data);

nvcAPI nvc::OutputGeomCache* nvcOGCCreate();
nvcAPI void nvcOGCRelease(nvc::OutputGeomCache *self);
nvcAPI int  nvcOGCGetMeshCount(nvc::OutputGeomCache *self);
nvcAPI void nvcOGCGetMesh(nvc::OutputGeomCache *self, int index, nvc::GeomMesh* dst);
nvcAPI int  nvcOGCGetSubmeshCount(nvc::OutputGeomCache *self);
nvcAPI void nvcOGCGetSubmesh(nvc::OutputGeomCache *self, int index, nvc::GeomSubmesh* dst);
nvcAPI int  nvcOGCGetVertexCount(nvc::OutputGeomCache *self);
nvcAPI int  nvcOGCGetIndexCount(nvc::OutputGeomCache *self);
nvcAPI int  nvcOGCCopyIndices(nvc::OutputGeomCache *self, const nvc::GeomSubmesh *gsm, int *dst);
nvcAPI int  nvcOGCCopyPoints(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float3 *dst);
nvcAPI int  nvcOGCCopyNormals(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float3 *dst);
nvcAPI int  nvcOGCCopyTangents(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float4 *dst);
nvcAPI int  nvcOGCCopyUV0(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float2 *dst);
nvcAPI int  nvcOGCCopyUV1(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float2 *dst);
nvcAPI int  nvcOGCCopyColors(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float4 *dst);

nvcAPI nvc::GeomCache* nvcGCCreate();
nvcAPI void nvcGCRelease(nvc::GeomCache *self);
nvcAPI int  nvcGCOpen(nvc::GeomCache *self, const char *path);
nvcAPI void nvcGCClose(nvc::GeomCache *self);
nvcAPI void nvcGCSetCurrentTime(nvc::GeomCache *self, float time);
nvcAPI int  nvcGCGetCurrentCache(nvc::GeomCache *self, nvc::OutputGeomCache *ogc);
