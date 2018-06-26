#pragma once

#include "../Types.h"
#include "../nvcAPI.h"

namespace nvc {

struct AlembicImportOptions
{
    // todo:
};

struct AlembicGeometries
{
    // GeomPtrs use nvcIGCRelease as deleter
    using GeomPtr = std::shared_ptr<nvc::InputGeomCache>;

    std::vector<GeomPtr> geometries;
};

bool AlembicToGeomCache(const char *path_to_abc, const AlembicImportOptions& options, AlembicGeometries& result);

} // namespace nvc
