#pragma once

class Stream;

namespace nvc
{
struct GeomCacheData;

class IDecompressor
{
public:
	IDecompressor() = default;
	virtual ~IDecompressor() = default;

	virtual void open(Stream* pStream) = 0;
	virtual void close() = 0;
	virtual void prefetch(size_t frameIndex, size_t range) = 0;

	virtual bool getData(size_t frameIndex, float& time, GeomCacheData& data) = 0;
	virtual bool getData(float time, GeomCacheData& data) = 0;

	//...
	IDecompressor(const IDecompressor&) = delete;
	IDecompressor(IDecompressor&&) = delete;
	IDecompressor& operator=(const IDecompressor&) = delete;
	IDecompressor& operator=(IDecompressor&&) = delete;
};

} // namespace nvc
