#pragma once

#include "../Types.h"
#include "../nvcAPI.h"

#define nvcSemantic_Points      "points"
#define nvcSemantic_Velocities  "velocities"
#define nvcSemantic_Normals     "normals"
#define nvcSemantic_Tangents    "tangents"
#define nvcSemantic_UV0         "uv0"
#define nvcSemantic_UV1         "uv1"
#define nvcSemantic_Colors      "colors"

namespace nvcabc {

struct AlembicImportOptions
{
    bool multithreading = true;
};

struct AlembicGeometries
{
    // GeomPtrs use nvcIGCRelease as deleter
    using GeomPtr = std::shared_ptr<nvc::InputGeomCache>;

    GeomPtr geometry;
    std::vector<float> timesamples;
};

bool AlembicToGeomCache(const char *path_to_abc, const AlembicImportOptions& options, AlembicGeometries& result);

} // namespace nvcabc
