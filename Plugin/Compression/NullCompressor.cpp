//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "NullCompressor.h"

//! Project Includes.
#include "Plugin/InputGeomCache.h"
#include "Plugin/Stream/Stream.h"

namespace nvc
{

//-----------------------------
// File
//-----------------------------
// Header
//  Descriptor
//    uint32_t itemCount;
//	  char[80] semantic;
//    DataFormat type;
//	  char[80] semantic;
//    DataFormat type;
//    ...
//  uint32_t frameCount;
//  uint32_t windowSize;
//
//-----------------------------
// Frame0: uint32_t offset;
// Frame[1*windowSize]: uint32_t offset;
// ...
// Frame[n*windowSize]: uint32_t offset;
//-----------------------------
// Frames:
//   I-Frame
//     FrameType Type;
//     uint32_t Count;
//     [DataBlob]
//   I-Frame
//     FrameType Type;
//     uint32_t Count;
//     [DataBlob]
// ...

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
	FileHeader header
	{
		header.FrameCount = static_cast<uint64_t>(geomCache.getDataCount()),
		header.VertexAttributeCount = vertexAttributeCount,
	};

	pStream->write(header);
	pStream->write(geomDesc, sizeof(GeomCacheDesc) * vertexAttributeCount);

	// Write frames.
	const uint64_t vertexDataSize = geomCache.getDataSize();
	for (uint64_t iFrame = 0; iFrame < header.FrameCount; ++iFrame)
	{
		float time = 0.0f;
		GeomCacheData frameData {};
		geomCache.getData(iFrame, time, &frameData);

		FrameHeader frameHeader 
		{
			frameHeader.Type = FrameType::IFrame,
			frameHeader.Time = time,
			frameHeader.VertexCount = frameData.count,
		};

		pStream->write(frameHeader);
		pStream->write(frameData.data, vertexDataSize * frameData.count);
	}
}

} //namespace nvc
