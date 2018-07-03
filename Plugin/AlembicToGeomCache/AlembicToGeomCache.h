#pragma once

#include "../nvcAPI.h"

#ifdef _WIN32
    #ifdef NVCABC_IMPL
        #define nvcabcAPI extern "C" __declspec(dllexport)
    #else
        #define nvcabcAPI extern "C" __declspec(dllimport)
    #endif
#else
    #define nvcabcAPI extern "C"
#endif


namespace nvcabc {
using namespace nvc;

enum class NormalsMode
{
    ReadFromFile,
    ComputeIfMissing,
    AlwaysCompute,
    Ignore
};

enum class TangentsMode
{
    None,
    Compute,
};

enum NodeType
{
    Unknwon,
    Xform,
    Camera,
    Light,
    Mesh,
    Points,
};

struct ImportOptions
{
    // options for abci
    NormalsMode normals_mode = NormalsMode::ComputeIfMissing;
    TangentsMode tangents_mode = TangentsMode::Compute;
    float scale_factor = 1.0f;
    float aspect_ratio = -1.0f;
    float vertex_motion_scale = 1.0f;
    int split_unit = 0x7fffffff;
    bool swap_handedness = true;
    bool swap_face_winding = false;
    bool interpolate_samples = true;
    bool turn_quad_edges = false;
    bool multithreading = true;

    bool import_point_polygon = true;
    bool import_line_polygon = true;
    bool import_triangle_polygon = true;


    // followings are extended options for nvcabc
    bool import_points = true;
};

struct ExportOptions
{
    // compression settings
    CompressionType compression_type = CompressionType::Quantize;
    int block_size = 30;
};

struct XformData
{
    float time = 0.0f;

    bool visibility = true;
    float3 translation = { 0.0f, 0.0f, 0.0f };
    float4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f }; // quaternion
    float3 scale = { 1.0f, 1.0f, 1.0f };
};

struct CameraData
{
    float time = 0.0f;

    bool visibility = true;
    float near_clipping_plane = 0.3f;
    float far_clipping_plane = 1000.0f;
    float field_of_view = 60.0f;        // in degree. vertical one
    float aspect_ratio = 16.0f / 9.0f;

    float focus_distance = 5.0f;        // in cm
    float focal_length = 0.0f;          // in mm
    float aperture = 2.4f;              // in cm. vertical one
};

class ImportContext;

} // namespace nvcabc

// just convert alembic file to InputGeomCache.
// ** caller must release returned object (call nvcIGCRelease()) **
nvcabcAPI nvc::InputGeomCache* nvcabcAlembicToInputGeomCache(const char *path_to_abc, const nvcabc::ImportOptions& options);


nvcabcAPI nvcabc::ImportContext* nvcabcCreateContext();
nvcabcAPI void nvcabcReleaseContext(nvcabc::ImportContext *self);
nvcabcAPI int nvcabcOpen(nvcabc::ImportContext *self, const char *path_to_abc, const nvcabc::ImportOptions* options);

// convert and export to file
nvcabcAPI int nvcabcExportNVC(nvcabc::ImportContext *self, const char *path_to_nvc, const nvcabc::ExportOptions* options);

nvcabcAPI int nvcabcGetNodeCount(nvcabc::ImportContext *self);
nvcabcAPI const char* nvcabcGetNodeName(nvcabc::ImportContext *self, int i);
nvcabcAPI const char* nvcabcGetNodePath(nvcabc::ImportContext *self, int i);
nvcabcAPI nvcabc::NodeType nvcabcGetNodeType(nvcabc::ImportContext *self, int i);
nvcabcAPI int nvcabcGetSampleCount(nvcabc::ImportContext *self, int i);
nvcabcAPI int nvcabcFillXformSamples(nvcabc::ImportContext *self, int i, nvcabc::XformData *dst);
nvcabcAPI int nvcabcFillCameraSamples(nvcabc::ImportContext *self, int i, nvcabc::CameraData *dst);
