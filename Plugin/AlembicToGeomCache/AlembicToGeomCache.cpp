#include "Plugin/PrecompiledHeader.h"
#include "AlembicToGeomCache.h"
#include "../../External/abci/abci.h"

namespace nvcabc {

using namespace nvc;

class ImportContext
{
public:
    void gatherMeshes(aiObject *obj);

private:
    std::vector<aiPolyMesh*> meshes;
    std::vector<int> indices;
    std::vector<nvc::float2> uv0, uv1;
    std::vector<nvc::float3> points, normals, velocities;
    std::vector<nvc::float4> colors;

};

static void WriteGeometry(aiPolyMesh *poly, AlembicGeometries& result)
{
    const char *path = aiObjectGetFullName((aiObject*)poly);

    aiMeshSummary summary;
    aiPolyMeshGetSummary(poly, &summary);

    aiMeshSampleSummary sample_summary;
    aiPolyMeshData idata;
    std::vector<aiSubmeshData> ismdata;

    // tmp vertex arrays
    std::vector<int> indices;
    std::vector<nvc::float2> uv0, uv1;
    std::vector<nvc::float3> points, normals, velocities;
    std::vector<nvc::float4> colors;
    int id_indices = 0, id_points = 0, id_velocities = 0, id_normals = 0, id_uv0 = 0, id_uv1 = 0, id_colors = 0;

    std::vector<std::function<void()>> tasks_allocate;
    std::vector<std::function<void()>> tasks_write;

    // make descriptors
    std::vector<nvc::GeomCacheDesc> descs;
    std::vector<nvc::GeomCacheData> odata;

    {
        id_indices = (int)descs.size();
        descs.push_back({ "indices", nvc::DataFormat::Int });
        odata.push_back({});

        tasks_allocate.push_back([&]() {
            indices.resize(sample_summary.index_count);
            idata.indices = indices.data();
        });
        tasks_write.push_back([&]() {
            auto& d = odata[id_indices];
            //d.count = indices.size();
            //d.data = indices.data();
        });
    }

    {
        id_points = (int)descs.size();
        descs.push_back({ "points", nvc::DataFormat::Float3 });
        odata.push_back({});

        tasks_allocate.push_back([&]() {
            points.resize(sample_summary.vertex_count);
            idata.points = (abcV3*)points.data();
        });
        tasks_write.push_back([&]() {
            auto& d = odata[id_points];
            //d.count = points.size();
            //d.data = points.data();
        });
    }

    if (summary.has_velocities) {
        id_velocities = (int)descs.size();
        descs.push_back({ "velocities", nvc::DataFormat::Float3 });
        odata.push_back({});

        tasks_allocate.push_back([&]() {
            velocities.resize(sample_summary.vertex_count);
            idata.velocities = (abcV3*)velocities.data();
        });
        tasks_write.push_back([&]() {
            auto& d = odata[id_velocities];
            //d.count = velocities.size();
            //d.data = velocities.data();
        });
    }

    if (summary.has_normals) {
        id_normals = (int)descs.size();
        descs.push_back({ "normals", nvc::DataFormat::Float3 });
        odata.push_back({});

        tasks_allocate.push_back([&]() {
            normals.resize(sample_summary.vertex_count);
            idata.normals = (abcV3*)normals.data();
        });
        tasks_write.push_back([&]() {
            auto& d = odata[id_normals];
            //d.count = normals.size();
            //d.data = normals.data();
        });
    }

    if (summary.has_uv0) {
        id_uv0 = (int)descs.size();
        descs.push_back({ "uv0", nvc::DataFormat::Float2 });
        odata.push_back({});

        tasks_allocate.push_back([&]() {
            uv0.resize(sample_summary.vertex_count);
            idata.uv0 = (abcV2*)uv0.data();
        });
        tasks_write.push_back([&]() {
            auto& d = odata[id_uv0];
            //d.count = uv0.size();
            //d.data = uv0.data();
        });
    }

    if (summary.has_uv1) {
        id_uv1 = (int)descs.size();
        descs.push_back({ "uv1", nvc::DataFormat::Float2 });
        odata.push_back({});

        tasks_allocate.push_back([&]() {
            uv1.resize(sample_summary.vertex_count);
            idata.uv1 = (abcV2*)uv1.data();
        });
        tasks_write.push_back([&]() {
            auto& d = odata[id_uv1];
            //d.count = uv1.size();
            //d.data = uv1.data();
        });
    }

    if (summary.has_colors) {
        id_colors = (int)descs.size();
        descs.push_back({ "colors", nvc::DataFormat::Float4 });
        odata.push_back({});

        tasks_allocate.push_back([&]() {
            colors.resize(sample_summary.vertex_count);
            idata.colors = (abcV4*)colors.data();
        });
        tasks_write.push_back([&]() {
            auto& d = odata[id_colors];
            //d.count = colors.size();
            //d.data = colors.data();
        });
    }

    descs.push_back(GEOM_CACHE_DESCRIPTOR_END);

    // create input geom cache!
    nvc::InputGeomCache *igc = nvcIGCCreate(path, descs.data());

    for (auto t : result.timesamples) {
        idata = {};

        auto ss = aiTimeToSampleSelector(t);
        aiSchemaUpdateSample(poly, &ss);
        auto sample = aiSchemaGetSample(poly);
        aiPolyMeshGetSampleSummary(sample, &sample_summary);
        ismdata.resize(sample_summary.submesh_count);

        for (auto& task : tasks_allocate)
            task();
        aiPolyMeshFillVertexBuffer(sample, &idata, ismdata.data());

        for (auto& task : tasks_write)
            task();

        nvcIGCAddData(igc, (float)t, odata.data());
    }

    //result.geometries.push_back(AlembicGeometries::GeomPtr(igc, nvcIGCRelease));
}

static void ProcessAbcObject(aiContext *ctx, aiObject *obj, AlembicGeometries& result)
{
    if (!obj)
        return;

    if (aiPolyMesh *poly = aiObjectAsPolyMesh(obj)) {
        WriteGeometry( poly, result);
    }

    // process children
    int num_children = aiObjectGetNumChildren(obj);
    for (int ci = 0; ci < num_children; ++ci) {
        ProcessAbcObject(ctx, aiObjectGetChild(obj, ci), result);
    }
}

bool AlembicToGeomCache(const char *path_to_abc, const AlembicImportOptions& /*options*/, AlembicGeometries& result)
{
    aiContext* ctx = aiContextCreate(1);
    if (!aiContextLoad(ctx, path_to_abc)) {
        aiContextDestroy(ctx);
        return false;
    }

    // gather sample times
    {
        int num_timesamplings = aiContextGetTimeSamplingCount(ctx);
        for (int tsi = 0; tsi < num_timesamplings; ++tsi) {
            auto *ts = aiContextGetTimeSampling(ctx, tsi);
            auto num_samples = aiTimeSamplingGetSampleCount(ts);
            for (int si = 0; si < num_samples; ++si) {
                auto t = aiTimeSamplingGetTime(ts, si);
                result.timesamples.push_back(t);
            }
        }
        std::sort(result.timesamples.begin(), result.timesamples.end());
        result.timesamples.erase(
            std::unique(result.timesamples.begin(), result.timesamples.end()),
            result.timesamples.end());
    }

    ProcessAbcObject(ctx, aiContextGetTopObject(ctx), result);

    aiContextDestroy(ctx);
    return true;
}

} // namespace nvcabc
