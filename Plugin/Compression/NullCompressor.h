#pragma once

//! Local Includes.
#include "ICompressor.h"

class NullCompressor final : public ICompressor
{
public:
	NullCompressor() = default;
	~NullCompressor() = default;

	void Compress(const InputGeomCache& geomCache, Stream* pStream) override;

	//...
	NullCompressor(const NullCompressor&) = delete;
	NullCompressor(NullCompressor&&) = delete;
	NullCompressor& operator=(const NullCompressor&) = delete;
	NullCompressor& operator=(NullCompressor&&) = delete;
};
