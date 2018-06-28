#pragma once
#include "Plugin/Types.h"

namespace nvc
{

namespace quantisation_compression
{
	struct FileHeader
	{
		uint64_t FrameCount;
		uint32_t FrameSeekWindowCount;
		uint32_t VertexAttributeCount;
		uint32_t ConstantDataSize;
	};

	struct FrameHeader
	{
		uint32_t IndexCount;
		uint32_t VertexCount;
	};

	static const uint32_t SEMANTIC_STRING_LENGTH = 60;
}

} // namespace nvc
