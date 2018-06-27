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
	GeomCacheDesc geomCacheDescs[GEOM_CACHE_MAX_DESCRIPTOR_COUNT + 1] {};
	igc.getDesc(&geomCacheDescs[0]);

//	const int descIndex_points   = getAttributeIndex(geomCacheDescs, SEMANTIC_POINTS  );
//	const int descIndex_normals  = getAttributeIndex(geomCacheDescs, SEMANTIC_NORMALS );
//	const int descIndex_tangents = getAttributeIndex(geomCacheDescs, SEMANTIC_TANGENTS);
//	const int descIndex_uv0      = getAttributeIndex(geomCacheDescs, SEMANTIC_UV0     );
//	const int descIndex_colors   = getAttributeIndex(geomCacheDescs, SEMANTIC_COLORS  );
	const int descIndex_points   = getAttributeIndex(geomCacheDescs, nvcSemantic_Points  );
	const int descIndex_normals  = getAttributeIndex(geomCacheDescs, nvcSemantic_Normals );
	const int descIndex_tangents = getAttributeIndex(geomCacheDescs, nvcSemantic_Tangents);
	const int descIndex_uv0      = getAttributeIndex(geomCacheDescs, nvcSemantic_UV0     );
	const int descIndex_colors   = getAttributeIndex(geomCacheDescs, nvcSemantic_Colors  );

	printf("GeomCacheDesc.descIndex_points   = %d\n", descIndex_points   );
	printf("GeomCacheDesc.descIndex_normals  = %d\n", descIndex_normals  );
	printf("GeomCacheDesc.descIndex_tangents = %d\n", descIndex_tangents );
	printf("GeomCacheDesc.descIndex_uv0      = %d\n", descIndex_uv0      );
	printf("GeomCacheDesc.descIndex_colors   = %d\n", descIndex_colors   );

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
