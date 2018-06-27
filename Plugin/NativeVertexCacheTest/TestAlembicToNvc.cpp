#include "Plugin/PrecompiledHeader.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"
#include "Plugin/Stream/FileStream.h"
#include "Plugin/Compression/NullCompressor.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/GeomCacheData.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"

static void dump(const nvc::InputGeomCache& igc) {
	using namespace nvc;
	using namespace nvcabc;
	const auto nFrame = igc.getDataCount();
	printf("nFrame(%zd)\n", nFrame);
	for(size_t iFrame = 0; iFrame < nFrame; ++iFrame) {
		printf("frame(%zd), ", iFrame);
		float frameTime = 0.0f;
		GeomCacheData gcd {};
		igc.getData(iFrame, frameTime, &gcd);

		printf("time(%5.3f)", frameTime);
		printf("\n");

		const auto nIndex = gcd.indexCount;
		printf("index[%zd]={", nIndex);
		const int32_t* pIndex = static_cast<const int32_t*>(gcd.indices);
		for(size_t iIndex = 0; iIndex < nIndex; ++iIndex) {
			const int32_t index = pIndex[iIndex];
			printf("%d, ", index);
		}
		printf("}\n");

		printf("\n");
	}
}

void RunTest_AlembicToNvc()
{
    using namespace nvc;
    using namespace nvcabc;

    const char* abcFilename = "../../../Data/Cloth-1frame.abc";
    const char* nvcFilename = "../../../Data/TestOutput/Cloth-1frame.nvc";

    assert(IsFileExist(abcFilename));
    RemoveFile(nvcFilename);

    AlembicImportOptions opt;
    AlembicGeometries abcGeoms;

    const auto abcToGcResult = AlembicToGeomCache(abcFilename, opt, abcGeoms);
    assert(abcToGcResult);

	dump(*abcGeoms.geometry);

    FileStream fs { nvcFilename, FileStream::OpenModes::Random_ReadWrite };
    NullCompressor nc {};
    nc.compress(*abcGeoms.geometry, &fs);
}
