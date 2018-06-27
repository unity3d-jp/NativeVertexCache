#include "Plugin/PrecompiledHeader.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"

void RunTest_Alembic()
{
    using namespace nvcabc;

    AlembicImportOptions opt;
    AlembicGeometries result;

    if (AlembicToGeomCache("Cloth.abc", opt, result)) {

    }
}
