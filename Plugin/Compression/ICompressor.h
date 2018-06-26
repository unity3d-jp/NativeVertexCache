#pragma once

class Stream;
class InputGeomCache;

class ICompressor
{
public:
	ICompressor() = default;
	virtual ~ICompressor() = default;

	virtual void Compress(const InputGeomCache& geomCache, Stream* pStream) = 0;

	//...
	ICompressor(const ICompressor&) = delete;
	ICompressor(ICompressor&&) = delete;
	ICompressor& operator=(const ICompressor&) = delete;
	ICompressor& operator=(ICompressor&&) = delete;
};
