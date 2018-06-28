#pragma once
namespace nvc {
	enum class AbcToNvcCompressionMethod : int32_t {
		Unknown,
		Null,
		Quantisation,
	};

	int AbcToNvc(const char* srcAbcFilename, const char* outNvcFilename, nvc::AbcToNvcCompressionMethod compressionMethod);
}
