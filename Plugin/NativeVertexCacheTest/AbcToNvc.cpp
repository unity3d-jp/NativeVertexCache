#include "Plugin/PrecompiledHeader.h"
#include "Plugin/Foundation/Types.h"
#include "Plugin/Foundation/Pcg.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"
#include "Plugin/Stream/FileStream.h"
#include "Plugin/Compression/NullCompressor.h"
#include "Plugin/Compression/QuantisationCompressor.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/GeomCache.h"
#include "Plugin/GeomCacheData.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"
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

	ImportOptions opt;
	const auto abcIgc = nvcabcAlembicToInputGeomCache(srcAbcFilename, opt);
	assert(abcIgc);

    RemoveFile(outNvcFilename);
	FileStream fs { outNvcFilename, FileStream::OpenModes::Random_ReadWrite };

	switch(compressionMethod) {
	default:
	case AbcToNvcCompressionMethod::Null:
		{
			NullCompressor nc {};
			nc.compress(*abcIgc, &fs);
		}
		break;
	case AbcToNvcCompressionMethod::Quantisation:
		{
			QuantisationCompressor qc {};
			qc.compress(*abcIgc, &fs);
		}
		break;
	}
    nvcIGCRelease(abcIgc);
    return 0;
}

} // namespace nvc
