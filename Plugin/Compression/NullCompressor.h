#pragma once

//! Local Includes.
#include "ICompressor.h"

namespace nvc
{

class NullCompressor final : public ICompressor
{
public:
	struct FileHeader
	{
		uint64_t FrameCount;
		uint64_t VertexAttributeCount;
	};

	enum class FrameType : uint32_t
	{
		IFrame,
	};

	struct FrameHeader
	{
		FrameType Type;
		float Time;
		uint64_t VertexCount;
	};

public:
	NullCompressor() = default;
	~NullCompressor() = default;

	void compress(const InputGeomCache& geomCache, Stream* pStream) override;

	//...
	NullCompressor(const NullCompressor&) = delete;
	NullCompressor(NullCompressor&&) = delete;
	NullCompressor& operator=(const NullCompressor&) = delete;
	NullCompressor& operator=(NullCompressor&&) = delete;
};

} // namespace nvc
