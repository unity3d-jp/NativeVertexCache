#include "Plugin/PrecompiledHeader.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"
#include "Plugin/Stream/FileStream.h"
#include "Plugin/Compression/NullCompressor.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"

void RunTest_AlembicToNvc()
{
    using namespace nvc;
    using namespace nvcabc;

    const char* abcFilename = "../../../Data/Cloth-1frame.abc";
    const char* nvcFilename = "../../../Data/Cloth-1frame.nvc";

    assert(IsFileExist(abcFilename));
    RemoveFile(nvcFilename);

    AlembicImportOptions opt;
    AlembicGeometries abcGeoms;

    const auto abcToGcResult = AlembicToGeomCache(abcFilename, opt, abcGeoms);
    assert(abcToGcResult);

    FileStream fs { nvcFilename, FileStream::OpenModes::Random_ReadWrite };
    NullCompressor nc {};
    nc.compress(*abcGeoms.geometry, &fs);
}
