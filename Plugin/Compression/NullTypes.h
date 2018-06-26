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

	enum class FrameType : uint32_t
	{
		IFrame,
	};

	struct FrameHeader
	{
		union
		{
			struct  
			{
				uint64_t Type : 8;
				uint64_t VertexCount : 56;
			};
			uint64_t DataValue;
		};
	};
}

} // namespace nvc
