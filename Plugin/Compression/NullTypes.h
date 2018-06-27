#pragma once

//! Local Includes.
#include "ICompressor.h"

namespace nvc
{

namespace null_compression
{
	struct FileHeader
	{
		uint64_t FrameCount;
		uint32_t FrameSeekWindowCount;
		uint32_t VertexAttributeCount;
	};

	struct FrameHeader
	{
		uint32_t IndexCount;
		uint32_t VertexCount;
	};

	struct MeshDesc
	{
		uint32_t MeshId;
		uint32_t vertexStart;
		uint32_t vertexCount;
		uint32_t indexStart;
		uint32_t indexCount;
	};

	static const uint32_t SEMANTIC_STRING_LENGTH = 60;
}

} // namespace nvc
