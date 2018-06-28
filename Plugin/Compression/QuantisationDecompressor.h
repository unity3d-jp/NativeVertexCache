#pragma once

//! Local Includes.
#include "IDecompressor.h"
#include "QuantisationTypes.h"

//! Project Includes.
#include "Plugin/GeomCacheData.h"

namespace nvc
{

class QuantisationDecompressor final : public IDecompressor
{
private:
	//using FrameDataType = std::pair<float, GeomCacheData>;

	Stream* m_pStream = nullptr;

	quantisation_compression::FileHeader m_Header = {};

	GeomCacheDesc m_Descriptor[GEOM_CACHE_MAX_DESCRIPTOR_COUNT] = {};
	char m_Semantics[GEOM_CACHE_MAX_DESCRIPTOR_COUNT][quantisation_compression::SEMANTIC_STRING_LENGTH] = {};

	std::vector<uint64_t> m_SeekTable;
	std::vector<float> m_FrameTimeTable;
	std::vector<bool> m_IsFrameLoaded;

	struct FrameDataType 
	{
		float Time;
		GeomCacheData Data;
	};

	std::vector<FrameDataType> m_LoadedFrames;

	size_t m_FramesOffset = 0;

public:
	QuantisationDecompressor() = default;
	~QuantisationDecompressor();

	void open(Stream* pStream) override;
	void close() override;
	void prefetch(size_t frameIndex, size_t range) override;

	bool getData(size_t frameIndex, float& time, GeomCacheData& data) override;
	bool getData(float time, GeomCacheData& data) override;
	const GeomCacheDesc* getDescriptors() const override { return &m_Descriptor[0]; }

	//...
	QuantisationDecompressor(const QuantisationDecompressor&) = delete;
	QuantisationDecompressor(QuantisationDecompressor&&) = delete;
	QuantisationDecompressor& operator=(const QuantisationDecompressor&) = delete;
	QuantisationDecompressor& operator=(QuantisationDecompressor&&) = delete;

private:
	size_t getSeekTableIndex(size_t frameIndex) const
	{
		return frameIndex / m_Header.FrameSeekWindowCount;
	}

	size_t getSeekTableOffset(size_t frameIndex) const
	{
		const size_t seekTableIndex = getSeekTableIndex(frameIndex);
		if (seekTableIndex < m_SeekTable.size())
		{
			return m_SeekTable[seekTableIndex];
		}

		assert(false);
		return ~0u;
	}

public:
	float getFrameTime(size_t frameIndex) const override
	{
		if (frameIndex < getFrameCount())
		{
			return m_FrameTimeTable[frameIndex];
		}

		return HUGE_VALF;
	}

	size_t getFrameIndex(float time) const override
	{
		const auto it = std::lower_bound(m_FrameTimeTable.cbegin(), m_FrameTimeTable.cend(), time);

		if (it != m_FrameTimeTable.end())
		{
			return (it - m_FrameTimeTable.begin());
		}

		return ~0u;
	}

	size_t getFrameCount() const override
	{
		return m_Header.FrameCount;
	}

private:
	void loadFrame(size_t frameIndex);
	void freeFrame(FrameDataType& data) const;

	bool insertLoadedData(size_t frameIndex, const FrameDataType& data);
};

} // namespace nvc
