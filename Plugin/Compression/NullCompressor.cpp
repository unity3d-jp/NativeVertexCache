//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "NullCompressor.h"

//! Project Includes.
#include "NullTypes.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/Stream/Stream.h"

namespace nvc
{

void NullCompressor::compress(const InputGeomCache& geomCache, Stream* pStream)
{
	GeomCacheDesc geomDesc[GEOM_CACHE_MAX_DESCRIPTOR_COUNT] = {};
	geomCache.getDesc(geomDesc);
	
	uint32_t vertexAttributeCount = 0;
	const GeomCacheDesc *currentDesc = geomDesc;
	while (currentDesc->semantic != nullptr)
	{
		++vertexAttributeCount;
	}

	// Write header.
	const null_compression::FileHeader header
	{
		static_cast<uint64_t>(geomCache.getDataCount()),
		10,
		vertexAttributeCount,
	};

	pStream->write(header);
	pStream->write(geomDesc, sizeof(GeomCacheDesc) * vertexAttributeCount);

	// Calculate frame offsets and write a dummy entry in the stream to hold the value later.
	const size_t frameSeekTableOffset = pStream->getPosition();
	const size_t frameSeekTableSize = (header.FrameCount + header.FrameSeekWindowCount - 1) / header.FrameSeekWindowCount + 1;
	for (uint64_t iEntry = 0; iEntry < frameSeekTableSize; ++iEntry)
	{
		pStream->write(static_cast<uint64_t>(0));
	}

	// Write frames.
	std::vector<uint64_t> frameSeekTableValues;

	// Write time array.
	for (uint64_t iFrame = 0; iFrame < header.FrameCount; ++iFrame)
	{
		float time = 0.0f;
		GeomCacheData frameData{};
		geomCache.getData(iFrame, time, &frameData);

		pStream->write(time);
	}

	// Write frames.
	const uint64_t vertexDataSize = geomCache.getDataSize();
	for (uint64_t iFrame = 0; iFrame < header.FrameCount; ++iFrame)
	{
		if ((iFrame % header.FrameSeekWindowCount) == 0)
		{
			frameSeekTableValues.push_back(pStream->getPosition());
		}

		float time = 0.0f;
		GeomCacheData frameData {};
		geomCache.getData(iFrame, time, &frameData);

		const null_compression::FrameHeader frameHeader 
		{
			static_cast<uint8_t>(null_compression::FrameType::IFrame),
			frameData.count,
		};

		pStream->write(frameHeader);
		pStream->write(frameData.data, vertexDataSize * frameData.count);
	}

	// Update the frame seek table with the real offsets.
	pStream->seek(frameSeekTableOffset, Stream::SeekOrigin::Begin);
	for (uint64_t iEntry = 0; iEntry < frameSeekTableSize; ++iEntry)
	{
		pStream->write(frameSeekTableValues[iEntry]);
	}
}

} //namespace nvc
