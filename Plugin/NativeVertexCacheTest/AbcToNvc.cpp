#include "Plugin/PrecompiledHeader.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"
#include "Plugin/Stream/FileStream.h"
#include "Plugin/Compression/NullCompressor.h"
#include "Plugin/Compression/QuantisationCompressor.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/GeomCache.h"
#include "Plugin/GeomCacheData.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"
#include "Plugin/Types.h"
#include "Plugin/Pcg.h"
#include "./AbcToNvc.h"

namespace nvc {

int AbcToNvc(const char* srcAbcFilename, const char* outNvcFilename, nvc::AbcToNvcCompressionMethod compressionMethod) {
    using namespace nvc;
    using namespace nvcabc;

	printf("srcAbcFilename(%s)\n", srcAbcFilename);
	printf("outNvcFilename(%s)\n", outNvcFilename);

	if(!srcAbcFilename || !outNvcFilename) {
		exit(EXIT_FAILURE);
	}

	assert(IsFileExist(srcAbcFilename));

	AlembicImportOptions opt;
	AlembicGeometries abcGeoms;
	const auto abcToGcResult = AlembicToGeomCache(srcAbcFilename, opt, abcGeoms);
	assert(abcToGcResult);

    RemoveFile(outNvcFilename);
	FileStream fs { outNvcFilename, FileStream::OpenModes::Random_ReadWrite };

	switch(compressionMethod) {
	default:
	case AbcToNvcCompressionMethod::Null:
		{
			NullCompressor nc {};
			nc.compress(*abcGeoms.geometry, &fs);
		}
		break;
	case AbcToNvcCompressionMethod::Quantisation:
		{
			QuantisationCompressor qc {};
			qc.compress(*abcGeoms.geometry, &fs);
		}
		break;
	}
	return 0;
}

} // namespace nvc
