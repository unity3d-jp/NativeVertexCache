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
		static_cast<uint32_t>(DefaultSeekWindow),
		static_cast<uint32_t>(getAttributeCount(geomDesc)),
	};

	pStream->write(header);

	// Write the descriptor.
	char buffer[null_compression::SEMANTIC_STRING_LENGTH] = {};
	for (uint32_t iAttribute = 0; iAttribute < header.VertexAttributeCount; ++iAttribute)
	{
		memset(buffer, 0, sizeof(buffer));
		assert(geomDesc[iAttribute].semantic != nullptr
			&& strlen(geomDesc[iAttribute].semantic) < null_compression::SEMANTIC_STRING_LENGTH);
		sprintf(buffer, "%s", geomDesc[iAttribute].semantic);

		pStream->write(buffer, sizeof(buffer));
		pStream->write<uint32_t>(static_cast<uint32_t>(geomDesc[iAttribute].format));
	}

	// Calculate frame offsets and write a dummy entry in the stream to hold the value later.
	const size_t frameSeekTableOffset = pStream->getPosition();
	const size_t frameSeekTableSize = (header.FrameCount + header.FrameSeekWindowCount - 1) / header.FrameSeekWindowCount;
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
		if (frameData.vertices == nullptr)
		{
			continue; // Error?
		}

		const null_compression::FrameHeader frameHeader
		{
			static_cast<uint32_t>(frameData.indexCount),
			static_cast<uint32_t>(frameData.vertexCount),
		};

		pStream->write(frameHeader);

		std::vector<null_compression::MeshHeader> meshes;
		const int meshIdIndex = getAttributeIndex(geomDesc, SEMANTIC_MESHID);
		if (meshIdIndex > 0)
		{
			int currentMeshId = 0;
			uint32_t currentMeshIndexStart = 0;
			uint32_t currentMeshVertexStart = 0;
			uint32_t currentMeshVertexEnd = 0;

			const int* meshIds = static_cast<const int*>(frameData.vertices[meshIdIndex]);
			const int* indices = static_cast<const int*>(frameData.indices);
			for (size_t i = 0; i < frameData.indexCount; ++i)
			{
				const int index = indices[i];
				if (meshIds[index] != currentMeshId)
				{
					meshes.push_back(null_compression::MeshHeader
						{
							static_cast<uint32_t>(currentMeshId),
							currentMeshVertexStart,
							(currentMeshVertexEnd - currentMeshVertexStart),
							currentMeshIndexStart,
							static_cast<uint32_t>(i - currentMeshIndexStart),
						});
					
					currentMeshIndexStart = static_cast<uint32_t>(i);
					currentMeshVertexStart = index;
					currentMeshVertexEnd = index;
					currentMeshId = meshIds[index];
				}

				currentMeshVertexStart = std::min(currentMeshVertexStart, static_cast<uint32_t>(index));
				currentMeshVertexEnd = std::max(currentMeshVertexEnd, static_cast<uint32_t>(index));
			}
		}
		else
		{
			meshes.push_back(null_compression::MeshHeader
				{ 
					0u, 
					static_cast<uint32_t>(frameData.vertexCount),
					0u,
					static_cast<uint32_t>(frameData.indexCount)
				});
		}

		// Write mesh data.
		pStream->write<uint64_t>(meshes.size());
		pStream->write(meshes.data(), sizeof(null_compression::MeshHeader) * meshes.size());

		// Write indices.
		if (frameData.indices)
		{
			pStream->write(frameData.indices, sizeof(int) * frameData.indexCount);
		}

		// Write vertices.
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
	for (uint64_t iEntry = 0; iEntry < frameSeekTableValues.size(); ++iEntry)
	{
		pStream->write(frameSeekTableValues[iEntry]);
	}
}

} //namespace nvc
