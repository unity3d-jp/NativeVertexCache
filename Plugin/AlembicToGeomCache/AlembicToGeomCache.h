#pragma once

#include "../Types.h"
#include "../InputGeomCache.h"

namespace nvc {

struct AlembicImportOptions
{
    // todo:
};

bool AlembicToGeomCache(const char *path_to_abc, const AlembicImportOptions& options);

} // namespace nvc
