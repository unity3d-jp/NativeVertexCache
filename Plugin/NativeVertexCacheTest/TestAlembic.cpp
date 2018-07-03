#include "Plugin/PrecompiledHeader.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"
#include "Plugin/InputGeomCache.h"


void RunTest_Alembic()
{
    using namespace nvcabc;

    ImportOptions opt;

    const char *path_to_abc = "Cloth.abc";

    auto *igc = nvcabcAlembicToInputGeomCache(path_to_abc, opt);
    if (igc) {
        printf("%s %p time samples\n", path_to_abc, igc);
        nvcIGCRelease(igc);
    }
    else {
        ThrowError("failed to open %s\n", path_to_abc);
    }
}
