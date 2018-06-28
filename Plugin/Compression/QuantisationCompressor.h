#pragma once

//! Local Includes.
#include "ICompressor.h"

namespace nvc
{

class QuantisationCompressor final : public ICompressor
{
public:
	static const size_t DefaultSeekWindow = 10;

public:
	QuantisationCompressor() = default;
	~QuantisationCompressor() = default;

	void compress(const InputGeomCache& geomCache, Stream* pStream) override;

	//...
	QuantisationCompressor(const QuantisationCompressor&) = delete;
	QuantisationCompressor(QuantisationCompressor&&) = delete;
	QuantisationCompressor& operator=(const QuantisationCompressor&) = delete;
	QuantisationCompressor& operator=(QuantisationCompressor&&) = delete;

private:
	void BuildAABB();
};

} // namespace nvc
