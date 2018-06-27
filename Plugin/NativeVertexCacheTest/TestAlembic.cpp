#include "Plugin/PrecompiledHeader.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"
#include "Plugin/InputGeomCache.h"


void RunTest_Alembic()
{
    using namespace nvcabc;

    AlembicImportOptions opt;
    AlembicGeometries result;

    const char *path_to_abc = "Cloth.abc";

    if (AlembicToGeomCache(path_to_abc, opt, result)) {
        printf("%s %d time samples\n", path_to_abc, (int)result.timesamples.size());
    }
    else {
        ThrowError("failed to open %s\n", path_to_abc);
    }
}
