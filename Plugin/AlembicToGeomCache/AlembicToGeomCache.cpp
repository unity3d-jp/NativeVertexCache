#include "Plugin/PrecompiledHeader.h"
#include "AlembicToGeomCache.h"
#include "../../External/abci/abci.h"

namespace nvc {

static AlembicGeometries::GeomPtr WriteGeometry(aiPolyMesh *poly)
{
    const char *path = aiObjectGetFullName((aiObject*)poly);

    aiMeshSummary summary;
    aiPolyMeshGetSummary(poly, &summary);

    // make descriptor
    std::vector<nvc::GeomCacheDesc> descs;
    descs.push_back({ "indices", nvc::DataFormat::Int });
    descs.push_back({ "points", nvc::DataFormat::Float3 });
    if (summary.has_velocities)
        descs.push_back({ "velocities", nvc::DataFormat::Float3 });
    if (summary.has_normals)
        descs.push_back({ "normals", nvc::DataFormat::Float3 });
    if (summary.has_uv0)
        descs.push_back({ "uv0", nvc::DataFormat::Float2 });
    if (summary.has_uv1)
        descs.push_back({ "uv1", nvc::DataFormat::Float2 });
    if (summary.has_colors)
        descs.push_back({ "colors", nvc::DataFormat::Float4 });
    descs.push_back(GEOM_CACHE_DESCRIPTOR_END);

    // create input geom cache!
    auto *igc = nvcIGCCreate(path, descs.data());


    // fill vertex arrays
    std::vector<int> indices;
    std::vector<nvc::float2> uv0, uv1;
    std::vector<nvc::float3> points, normals, velocities;
    std::vector<nvc::float4> colors;




    return AlembicGeometries::GeomPtr(igc, nvcIGCRelease);
}

static void ProcessAbcObject(aiObject *obj, AlembicGeometries& result)
{
    if (!obj)
        return;

    aiPolyMesh *poly = aiObjectAsPolyMesh(obj);
    if (poly) {
        result.geometries.push_back(WriteGeometry(poly));
    }

    // process children
    int num_children = aiObjectGetNumChildren(obj);
    for (int ci = 0; ci < num_children; ++ci) {
        ProcessAbcObject(aiObjectGetChild(obj, ci), result);
    }
}

bool AlembicToGeomCache(const char *path_to_abc, const AlembicImportOptions& /*options*/, AlembicGeometries& result)
{
    aiContext* ctx = aiContextCreate(1);
    if (!aiContextLoad(ctx, path_to_abc)) {
        aiContextDestroy(ctx);
        return false;
    }

    ProcessAbcObject(aiContextGetTopObject(ctx), result);

    aiContextDestroy(ctx);
    return true;
}

} // namespace nvc
