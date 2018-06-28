#pragma once
#include "Plugin/PrecompiledHeader.h"
#include "Plugin/OutputGeomCache.h"
#include "Plugin/Utils.h"

namespace nvc {

void OutputGeomCache::copyIndices(const GeomSubmesh & subm, int * dst)
{
    if (!indices.empty()) {
        nvc::copy(&indices[subm.indexOffset], dst, subm.indexCount);
    }
}

void OutputGeomCache::copyPoints(const GeomMesh & mesh, float3 * dst)
{
    if (!points.empty()) {
        nvc::copy(&points[mesh.vertexOffset], dst, mesh.vertexCount);
    }
}

void OutputGeomCache::copyNormals(const GeomMesh & mesh, float3 * dst)
{
    if (!normals.empty()) {
        nvc::copy(&normals[mesh.vertexOffset], dst, mesh.vertexCount);
    }
}

void OutputGeomCache::copyTangents(const GeomMesh & mesh, float4 * dst)
{
    if (!tangents.empty()) {
        nvc::copy(&tangents[mesh.vertexOffset], dst, mesh.vertexCount);
    }
}

void OutputGeomCache::copyUV0(const GeomMesh & mesh, float2 * dst)
{
    if (!uv0.empty()) {
        nvc::copy(&uv0[mesh.vertexOffset], dst, mesh.vertexCount);
    }
}

void OutputGeomCache::copyUV1(const GeomMesh & mesh, float2 * dst)
{
    if (!uv1.empty()) {
        nvc::copy(&uv1[mesh.vertexOffset], dst, mesh.vertexCount);
    }
}

void OutputGeomCache::copyColors(const GeomMesh & mesh, float4 * dst)
{
    if (!colors.empty()) {
        nvc::copy(&colors[mesh.vertexOffset], dst, mesh.vertexCount);
    }
}

} // namespace nvc
