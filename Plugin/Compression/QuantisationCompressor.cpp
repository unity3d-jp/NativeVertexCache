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

	// Find vertex attributes and update the formats.
	size_t pointsAttributeIndex = ~0u;
	size_t velocitiesAttributeIndex = ~0u;
	size_t normalsAttributeIndex = ~0u;
	size_t tangentsAttributeIndex = ~0u;
	size_t uv0AttributeIndex = ~0u;
	size_t uv1AttributeIndex = ~0u;
	size_t vertexIdAttributeIndex = ~0u;
	size_t meshIdAttributeIndex = ~0u;

	const size_t attributeCount = getAttributeCount(geomDesc);
	for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
	{
		if (geomDesc[iAttribute].semantic != nullptr)
		{
			if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_POINTS) == 0)
			{
				pointsAttributeIndex = iAttribute;
				geomDesc[iAttribute].format = DataFormat::UNorm16x3;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_VELOCITIES) == 0)
			{
				velocitiesAttributeIndex = iAttribute;
				geomDesc[iAttribute].format = DataFormat::UNorm16x3;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_NORMALS) == 0)
			{
				normalsAttributeIndex = iAttribute;
				geomDesc[iAttribute].format = DataFormat::UNorm16x2;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_TANGENTS) == 0)
			{
				tangentsAttributeIndex = iAttribute;
				geomDesc[iAttribute].format = DataFormat::UNorm16x2;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_UV0) == 0)
			{
				uv0AttributeIndex = iAttribute;
				geomDesc[iAttribute].format = DataFormat::UNorm16x2;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_UV1) == 0)
			{
				uv1AttributeIndex = iAttribute;
				geomDesc[iAttribute].format = DataFormat::UNorm16x2;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_VERTEXID) == 0)
			{
				vertexIdAttributeIndex = iAttribute;
			}
			else if (_stricmp(geomDesc[iAttribute].semantic, nvcSEMANTIC_MESHID) == 0)
			{
				meshIdAttributeIndex = iAttribute;
			}
		}
	}

	// Check if we need UV0, UV1 or velocities.
	bool areUV0Null = true;
	bool areUV1Null = true;
	bool areVelocitiesNull = true;

	size_t frameCount = geomCache.getDataCount();
	for (uint64_t iFrame = 0; iFrame < frameCount; ++iFrame)
	{
		float time = 0.0f;
		GeomCacheData frameData{};
		geomCache.getData(iFrame, time, &frameData);
		if (frameData.vertices == nullptr)
		{
			continue; // Error?
		}

		// Write vertices.
		if (frameData.vertices)
		{
			if (uv0AttributeIndex == ~0u)
			{
				const float2* uv0 = static_cast<const float2*>(frameData.vertices[uv0AttributeIndex]);
				for (size_t iVertex = 0; areUV0Null && (iVertex < frameData.vertexCount); ++iVertex)
				{
					areUV0Null = areUV0Null && (uv0[iVertex][0] == 0.0f && uv0[iVertex][1] == 0.0f);
				}
			}

			if (uv1AttributeIndex == ~0u)
			{
				const float2* uv1 = static_cast<const float2*>(frameData.vertices[uv1AttributeIndex]);
				for (size_t iVertex = 0; areUV1Null && (iVertex < frameData.vertexCount); ++iVertex)
				{
					areUV1Null = areUV1Null && (uv1[iVertex][0] == 0.0f && uv1[iVertex][1] == 0.0f);
				}
			}

			if (velocitiesAttributeIndex == ~0u)
			{
				const float3* velocities = static_cast<const float3*>(frameData.vertices[velocitiesAttributeIndex]);
				for (size_t iVertex = 0; areVelocitiesNull && (iVertex < frameData.vertexCount); ++iVertex)
				{
					areVelocitiesNull = areVelocitiesNull
						&& (velocities[iVertex][0] == 0.0f
							&& velocities[iVertex][1] == 0.0f
							&& velocities[iVertex][2] == 0.0f);
				}
			}
		}
	}

	uint32_t attributeToRemove = 0;
	if (uv0AttributeIndex != ~0u && areUV0Null)
	{
		++attributeToRemove;
	}

	if (uv1AttributeIndex != ~0u && areUV1Null)
	{
		++attributeToRemove;
	}

	if (velocitiesAttributeIndex != ~0u && areVelocitiesNull)
	{
		++attributeToRemove;
	}

	if (vertexIdAttributeIndex != ~0u)
	{
		++attributeToRemove;
	}
	if (meshIdAttributeIndex != ~0u)
	{
		++attributeToRemove;
	}

	// Write header.
	const quantisation_compression::FileHeader header
	{
		static_cast<uint64_t>(geomCache.getDataCount()),
		static_cast<uint32_t>(DefaultSeekWindow),
		static_cast<uint32_t>(getAttributeCount(geomDesc)) - attributeToRemove,
		static_cast<uint32_t>(geomConstantData.getSizeAsByteArray())
	};

	pStream->write(header);

	// Write the descriptor.
	char buffer[quantisation_compression::SEMANTIC_STRING_LENGTH] = {};
	for (uint32_t iAttribute = 0; iAttribute < header.VertexAttributeCount; ++iAttribute)
	{
		if (iAttribute == vertexIdAttributeIndex
			|| iAttribute == meshIdAttributeIndex
			|| (iAttribute == velocitiesAttributeIndex && areVelocitiesNull)
			|| (iAttribute == uv0AttributeIndex && areUV0Null)
			|| (iAttribute == uv1AttributeIndex && areUV1Null))
		{
			// Skip.
		}
		else
		{
			memset(buffer, 0, sizeof(buffer));
			assert(geomDesc[iAttribute].semantic != nullptr
				&& strlen(geomDesc[iAttribute].semantic) < quantisation_compression::SEMANTIC_STRING_LENGTH);
			sprintf(buffer, "%s", geomDesc[iAttribute].semantic);

			pStream->write(buffer, sizeof(buffer));
			pStream->write<uint32_t>(static_cast<uint32_t>(geomDesc[iAttribute].format));
		}
	}

	// Calculate frame offsets and write a dummy entry in the stream to hold the value later.
	const size_t frameSeekTableOffset = pStream->getPosition();
	const size_t frameSeekTableSize = (header.FrameCount + header.FrameSeekWindowCount - 1) / header.FrameSeekWindowCount;
	for (uint64_t iEntry = 0; iEntry < frameSeekTableSize; ++iEntry)
	{
		pStream->write(static_cast<uint64_t>(0));
	}

	// Write constant data.
	if(header.ConstantDataSize > 0)
	{
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

			pStream->write(verticesAABB);

			for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
			{
				if (iAttribute == vertexIdAttributeIndex
					|| iAttribute == meshIdAttributeIndex
					|| (iAttribute == velocitiesAttributeIndex && areVelocitiesNull)
					|| (iAttribute == uv0AttributeIndex && areUV0Null)
					|| (iAttribute == uv1AttributeIndex && areUV1Null))
				{
					// Skip.
				}
				else if (iAttribute == pointsAttributeIndex)
				{
					unorm16x3* packedVertices = new unorm16x3[frameData.vertexCount];
					for (size_t iVertex = 0; iVertex < frameData.vertexCount; ++iVertex)
					{
						packedVertices[iVertex] = PackPoint(verticesAABB, points[iVertex]);
					}

					const size_t dataSize = getSizeOfDataFormat(DataFormat::UNorm16x3) * frameData.vertexCount;
					pStream->write(packedVertices, dataSize);

					delete[] packedVertices;
				}
				else if (iAttribute == velocitiesAttributeIndex)
				{
					float3* velocities = static_cast<float3*>(frameData.vertices[iAttribute]);
					unorm16x3* packedVelocities = new unorm16x3[frameData.vertexCount];
					for (size_t iVelocities = 0; iVelocities < frameData.vertexCount; ++iVelocities)
					{
						packedVelocities[iVelocities] = PackPoint(verticesAABB, velocities[iVelocities]);
					}

					const size_t dataSize = getSizeOfDataFormat(DataFormat::UNorm16x3) * frameData.vertexCount;
					pStream->write(packedVelocities, dataSize);

					delete[] packedVelocities;
				}
				else if (iAttribute == normalsAttributeIndex)
				{
					float3* normals = static_cast<float3*>(frameData.vertices[iAttribute]);

					unorm16x2* packedNormals = new unorm16x2[frameData.vertexCount];
					for (size_t iVertex = 0; iVertex < frameData.vertexCount; ++iVertex)
					{
						float2 n = OctEncode(normals[iVertex]);
						packedNormals[iVertex][0] = n[0];
						packedNormals[iVertex][1] = n[1];
					}

					const size_t dataSize = getSizeOfDataFormat(DataFormat::UNorm16x2) * frameData.vertexCount;
					pStream->write(packedNormals, dataSize);

					delete[] packedNormals;
				}
				else if (iAttribute == tangentsAttributeIndex)
				{
					float4* tangents = static_cast<float4*>(frameData.vertices[iAttribute]);

					unorm16x2* packedTangents = new unorm16x2[frameData.vertexCount];
					for (size_t iVertex = 0; iVertex < frameData.vertexCount; ++iVertex)
					{
						float2 t = OctEncode(tangents[iVertex]);
						packedTangents[iVertex][0] = t[0];
						packedTangents[iVertex][1] = t[1];
					}

					const size_t dataSize = getSizeOfDataFormat(DataFormat::UNorm16x2) * frameData.vertexCount;
					pStream->write(packedTangents, dataSize);

					delete[] packedTangents;
				}
				else if (iAttribute == uv0AttributeIndex
					|| iAttribute == uv1AttributeIndex)
				{
					float2* uvs = static_cast<float2*>(frameData.vertices[iAttribute]);

					unorm16x2* packedUVs = new unorm16x2[frameData.vertexCount];
					for (size_t iVertex = 0; iVertex < frameData.vertexCount; ++iVertex)
					{
						packedUVs[iVertex][0] = uvs[iVertex][0];
						packedUVs[iVertex][1] = uvs[iVertex][1];
					}

					const size_t dataSize = getSizeOfDataFormat(DataFormat::UNorm16x2) * frameData.vertexCount;
					pStream->write(packedUVs, dataSize);

					delete[] packedUVs;
				}
				else
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
