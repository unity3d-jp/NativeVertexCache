#pragma once

//! Local Includes.
#include "ICompressor.h"

namespace nvc
{

class NullCompressor final : public ICompressor
{
public:
	static const size_t DefaultSeekWindow = 10;

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
