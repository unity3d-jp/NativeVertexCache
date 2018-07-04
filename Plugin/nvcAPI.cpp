#include "Plugin/PrecompiledHeader.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/OutputGeomCache.h"
#include "Plugin/GeomCache.h"
#include "Plugin/nvcAPI.h"


nvcAPI nvc::InputGeomCache* nvcIGCCreate(const nvc::GeomCacheDesc *descs, const nvc::InputGeomCacheConstantData* constants)
{
    return new nvc::InputGeomCache(descs, constants);
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
nvcAPI void nvcIGCAClearData(nvc::InputGeomCache * self)
{
    if (self) {
        self->clearData();
    }
}

nvcAPI nvc::InputGeomCacheConstantData* nvcIGCCreateConstantData()
{
    return new nvc::InputGeomCacheConstantData();
}
nvcAPI void nvcIGCReleaseConstantData(nvc::InputGeomCacheConstantData* self)
{
    delete self;
}
nvcAPI int nvcIGCAddConstantDataString(nvc::InputGeomCacheConstantData* self, const char* str)
{
	if(self) {
		return static_cast<int>(self->addString(str));
	}
	return -1;
}


nvcAPI nvc::OutputGeomCache* nvcOGCCreate()
{
    return new nvc::OutputGeomCache();
}

nvcAPI void nvcOGCRelease(nvc::OutputGeomCache *self)
{
    delete self;
}

nvcAPI int nvcOGCGetMeshCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->meshes.size() : 0;
}
nvcAPI void nvcOGCGetMesh(nvc::OutputGeomCache *self, int index, nvc::GeomMesh* dst)
{
    if (self) {
        *dst = self->meshes[index];
    }
}
nvcAPI int nvcOGCGetSubmeshCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->submeshes.size() : 0;
}
nvcAPI void nvcOGCGetSubmesh(nvc::OutputGeomCache *self, int index, nvc::GeomSubmesh* dst)
{
    if (self) {
        *dst = self->submeshes[index];
    }
}

nvcAPI int nvcOGCGetVertexCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->points.size() : 0;
}

nvcAPI int nvcOGCGetIndexCount(nvc::OutputGeomCache *self)
{
    return self ? (int)self->indices.size() : 0;
}

nvcAPI int nvcOGCCopyIndices(nvc::OutputGeomCache *self, const nvc::GeomSubmesh *gsm, int *dst)
{
    if (self) {
        return self->copyIndices(*gsm, dst);
    }
    return false;
}

nvcAPI int nvcOGCCopyPoints(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float3 *dst)
{
    if (self) {
        return self->copyPoints(*gm, dst);
    }
    return false;
}

nvcAPI int nvcOGCCopyNormals(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float3 *dst)
{
    if (self) {
        return self->copyNormals(*gm, dst);
    }
    return false;
}

nvcAPI int nvcOGCCopyTangents(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float4 *dst)
{
    if (self) {
        return self->copyTangents(*gm, dst);
    }
    return false;
}

nvcAPI int nvcOGCCopyUV0(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float2 *dst)
{
    if (self) {
        return self->copyUV0(*gm, dst);
    }
    return false;
}

nvcAPI int nvcOGCCopyUV1(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float2 *dst)
{
    if (self) {
        return self->copyUV1(*gm, dst);
    }
    return false;
}

nvcAPI int nvcOGCCopyColors(nvc::OutputGeomCache *self, const nvc::GeomMesh *gm, nvc::float4 *dst)
{
    if (self) {
        return self->copyColors(*gm, dst);
    }
    return false;
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

nvcAPI int nvcGCGetCurrentCache(nvc::GeomCache *self, nvc::OutputGeomCache *ogc)
{
    if (self && ogc) {
        return self->assignCurrentDataToMesh(*ogc);
    }
    return false;
}

nvcAPI int  nvcGCGetConstantDataStringSize(nvc::GeomCache *self)
{
	if(self) {
		return static_cast<int>(self->getConstantDataStringSize());
	}
	return -1;
}

nvcAPI const char*  nvcGCGetConstantDataString(nvc::GeomCache *self, int index)
{
	if(self) {
		return self->getConstantDataString(index);
	}
	return nullptr;
}
