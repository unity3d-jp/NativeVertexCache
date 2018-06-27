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
	
	// Write header.
	const null_compression::FileHeader header
	{
		static_cast<uint64_t>(geomCache.getDataCount()),
		10,
		getAttributeCount(geomDesc),
	};

	pStream->write(header);
	pStream->write(geomDesc, sizeof(GeomCacheDesc) * header.VertexAttributeCount);

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
	const size_t attributeCount = getAttributeCount(geomDesc);

	for (uint64_t iFrame = 0; iFrame < header.FrameCount; ++iFrame)
	{
		if ((iFrame % header.FrameSeekWindowCount) == 0)
		{
			frameSeekTableValues.push_back(pStream->getPosition());
		}

		float time = 0.0f;
		GeomCacheData frameData{};
		geomCache.getData(iFrame, time, &frameData);

		const null_compression::FrameHeader frameHeader
		{
			frameData.indexCount,
			frameData.vertexCount,
		};

		pStream->write(frameHeader);
		if (frameData.indices)
		{
			pStream->write(frameData.indices, sizeof(int) * frameData.indexCount);
		}

		if (frameData.vertices)
		{
			for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
			{
				const size_t dataSize = getSizeOfDataFormat(geomDesc[iAttribute].format) * frameData.vertexCount;
				pStream->write(frameData.vertices[iAttribute], dataSize);
			}
		}
	}

	// Update the frame seek table with the real offsets.
	pStream->seek(frameSeekTableOffset, Stream::SeekOrigin::Begin);
	for (uint64_t iEntry = 0; iEntry < frameSeekTableSize; ++iEntry)
	{
		pStream->write(frameSeekTableValues[iEntry]);
	}
}

} //namespace nvc
