#pragma once

#include "./nvcabcAPI.h"
#include "Plugin/Foundation/RawVector.h"
#include "External/abci/abci.h"

namespace nvcabc {

using namespace nvc;

class ImportContext;

class MeshSegment
{
public:
    MeshSegment(ImportContext *ctx, aiPolyMesh *abc);
    void onUpdateSample();
    void getCounts(size_t& index_count, size_t& vertex_count, size_t& subm_count);
    void setOffsets(size_t index_offset, size_t vertex_offset, size_t subm_offset);
    void fillVertexBuffersBegin();
    void fillVertexBuffersEnd();

public:
    ImportContext * m_ctx;
    aiPolyMesh * m_mesh;
    aiMeshSummary m_summary;
    aiMeshSampleSummary m_sample_summary;
    std::vector<aiSubmeshSummary> m_submesh_summaries;
    std::vector<aiSubmeshData> m_submeshes;
    aiPolyMeshData m_vdata;
    size_t index_offset = 0;
    size_t vertex_offset = 0;
    size_t subm_offset = 0;
};


class ImportContext
{
public:
    using InputGeomPtr = std::shared_ptr<nvc::InputGeomCache>;

    ImportContext();
    ~ImportContext();

    void clear();
    bool open(const char *path, const ImportOptions& opt);

    void gatherTimes();
    void gatherMeshes();
    InputGeomCache* gatherSamples();

private:
    void gatherMeshes(aiObject *obj);
    void gatherSamples(double time, nvc::InputGeomCache *igc);

    aiContext * m_ctx = nullptr;
    InputGeomCacheConstantData *m_igcconst = nullptr;
    std::vector<MeshSegment> m_segments;

public:
    std::vector<aiObject*> m_abc_nodes;
    std::vector<nvc::GeomCacheDesc> m_descs;
    std::vector<nvc::GeomMesh> m_geomeshes;
    std::vector<nvc::GeomSubmesh> m_geosubmeshes;
    std::vector<void*> m_data_pointers;

    RawVector<double> m_timesamples;
    RawVector<int> m_indices;
    RawVector<nvc::float2> m_uv0, m_uv1;
    RawVector<nvc::float3> m_points, m_normals, m_velocities;
    RawVector<nvc::float4> m_tangents, m_colors;
    int m_id_points = -1,
        m_id_velocities = -1,
        m_id_normals = -1,
        m_id_tangents = -1,
        m_id_uv0 = -1,
        m_id_uv1 = -1,
        m_id_colors = -1;

};

} // namespace nvcabc
