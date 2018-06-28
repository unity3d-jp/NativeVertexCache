//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "QuantisationCompressor.h"

//! Project Includes.
#include "QuantisationTypes.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/Stream/Stream.h"
#include "Plugin/Types.h"
#include "PackedTransform.h"

namespace nvc
{

void QuantisationCompressor::compress(const InputGeomCache& geomCache, Stream* pStream)
{
	GeomCacheDesc geomDesc[GEOM_CACHE_MAX_DESCRIPTOR_COUNT] = {};
	geomCache.getDesc(geomDesc);

	InputGeomCacheConstantData geomConstantData {};
	geomCache.getConstantData(geomConstantData);

	// Write header.
	const quantisation_compression::FileHeader header
	{
		static_cast<uint64_t>(geomCache.getDataCount()),
		static_cast<uint32_t>(DefaultSeekWindow),
		static_cast<uint32_t>(getAttributeCount(geomDesc)),
		static_cast<uint32_t>(geomConstantData.getSizeAsByteArray())
	};

	pStream->write(header);

	// Write the descriptor.
	char buffer[quantisation_compression::SEMANTIC_STRING_LENGTH] = {};
	for (uint32_t iAttribute = 0; iAttribute < header.VertexAttributeCount; ++iAttribute)
	{
		memset(buffer, 0, sizeof(buffer));
		assert(geomDesc[iAttribute].semantic != nullptr
			&& strlen(geomDesc[iAttribute].semantic) < quantisation_compression::SEMANTIC_STRING_LENGTH);
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

	// Write constant data.
	if(header.ConstantDataSize > 0) {
		geomConstantData.storeDataTo(pStream);
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

	// Find vertex attributes.
	size_t pointsAttributeIndex = ~0u;
	size_t normalsAttributeIndex = ~0u;
	size_t tangentsAttributeIndex = ~0u;

	const size_t attributeCount = getAttributeCount(geomDesc);
	for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
	{
		if (geomDesc[iAttribute].semantic != nullptr)
		{
			if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_POINTS) == 0)
			{
				pointsAttributeIndex = iAttribute;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_NORMALS) == 0)
			{
				normalsAttributeIndex = iAttribute;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_TANGENTS) == 0)
			{
				tangentsAttributeIndex = iAttribute;
			}
		}
	}

	//struct ChannelsData
	//{
	//	float3 *points;
	//	float3 *normals;
	//	float4 *tangents;
	//};

	//ChannelsData cacheData;


	// Write frames.
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

		const quantisation_compression::FrameHeader frameHeader
		{
			static_cast<uint32_t>(frameData.indexCount),
			static_cast<uint32_t>(frameData.vertexCount),
		};

		pStream->write(frameHeader);

		// Write mesh and submesh data.
		pStream->write<uint64_t>(frameData.meshCount);
		pStream->write(frameData.meshes, sizeof(GeomMesh) * frameData.meshCount);

        pStream->write<uint64_t>(frameData.submeshCount);
        pStream->write(frameData.submeshes, sizeof(GeomSubmesh) * frameData.submeshCount);

		// Write indices.
		if (frameData.indices)
		{
			pStream->write(frameData.indices, sizeof(int) * frameData.indexCount);
		}

		// Write vertices.
		if (frameData.vertices)
		{
			float3* points = static_cast<float3*>(frameData.vertices[pointsAttributeIndex]);
			const AABB verticesAABB = AABB::Build(points, frameData.vertexCount);

			// Save position and [normals + tangents] as transform.
			PackedTransform* packedTransform = new PackedTransform[frameData.vertexCount];

			if (pointsAttributeIndex != ~0u)
			{
				for (size_t iVertex = 0; iVertex < frameData.vertexCount; ++iVertex)
				{
					packedTransform[iVertex].Translation = PackPoint(verticesAABB, points[iVertex]);
				}
			}

			if (normalsAttributeIndex != ~0u)
			{
				float3* normals = static_cast<float3*>(frameData.vertices[normalsAttributeIndex]);
				for (size_t iVertex = 0; iVertex < frameData.vertexCount; ++iVertex)
				{
					float2 n = OctEncode(normals[iVertex]);
					packedTransform[iVertex].Normal[0] = n[0];
					packedTransform[iVertex].Normal[1] = n[1];
				}
			}

			if (tangentsAttributeIndex != ~0u)
			{
				float4* tangents = static_cast<float4*>(frameData.vertices[tangentsAttributeIndex]);
				for (size_t iVertex = 0; iVertex < frameData.vertexCount; ++iVertex)
				{
					float2 t = OctEncode(tangents[iVertex]);
					packedTransform[iVertex].Tangent[0] = t[0];
					packedTransform[iVertex].Tangent[1] = t[1];
				}
			}

			for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
			{
				if (iAttribute != pointsAttributeIndex
					&& iAttribute != normalsAttributeIndex
					&& iAttribute != tangentsAttributeIndex)
				{
					const size_t dataSize = getSizeOfDataFormat(geomDesc[iAttribute].format) * frameData.vertexCount;
					pStream->write(frameData.vertices[iAttribute], dataSize);
				}
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
