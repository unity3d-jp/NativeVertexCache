//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "QuantisationDecompressor.h"

//! Project Includes.
#include "Plugin/Stream/Stream.h"
#include "Plugin/InputGeomCache.h"
#include "PackedTransform.h"

namespace nvc
{

QuantisationDecompressor::~QuantisationDecompressor()
{
	close();
}

void QuantisationDecompressor::open(Stream* pStream)
{
	close();

	m_pStream = pStream;

	// Read the file header.
	m_pStream->read(m_Header);

	// Read the descriptor.
	for (uint32_t iElement = 0; iElement < m_Header.VertexAttributeCount; ++iElement)
	{
		m_pStream->read(m_Semantics[iElement], sizeof(char) * quantisation_compression::SEMANTIC_STRING_LENGTH);

		uint32_t format = 0;
		m_pStream->read(format);

		m_Descriptor[iElement].semantic = m_Semantics[iElement];
		m_Descriptor[iElement].format = static_cast<DataFormat>(format);

		if (_stricmp(m_Descriptor[iElement].semantic, nvcSEMANTIC_POINTS) == 0)
		{
			m_Descriptor[iElement].format = DataFormat::Float3;
		}
		else if (_stricmp(m_Descriptor[iElement].semantic, nvcSEMANTIC_NORMALS) == 0)
		{
			m_Descriptor[iElement].format = DataFormat::Float3;
		}
		else if (_stricmp(m_Descriptor[iElement].semantic, nvcSEMANTIC_TANGENTS) == 0)
		{
			m_Descriptor[iElement].format = DataFormat::Float4;
		}
		else if (_stricmp(m_Descriptor[iElement].semantic, nvcSEMANTIC_UV0) == 0
			|| _stricmp(m_Descriptor[iElement].semantic, nvcSEMANTIC_UV1) == 0)
		{
			m_Descriptor[iElement].format = DataFormat::Float2;
		}
	}

	m_FramesOffset = m_pStream->getPosition();

	// Read the seek table.
	const size_t seekTableSize = (m_Header.FrameCount + m_Header.FrameSeekWindowCount - 1) / m_Header.FrameSeekWindowCount;
	for (uint64_t iEntry = 0; iEntry < seekTableSize; ++iEntry)
	{
		m_SeekTable.push_back(m_pStream->read<uint64_t>());
	}

	// Read constant data
	if(m_Header.ConstantDataSize > 0)
	{
		m_ConstantData.resize(m_Header.ConstantDataSize);
		m_pStream->read(m_ConstantData.data(), m_ConstantData.size());
	}

	// Read the time table.
	for (size_t iFrame = 0; iFrame < m_Header.FrameCount; ++iFrame)
	{
		m_FrameTimeTable.push_back(m_pStream->read<float>());
	}

	m_IsFrameLoaded.resize(m_Header.FrameCount, false);
}

void QuantisationDecompressor::close()
{
	auto it = m_LoadedFrames.begin();
	const auto itEnd = m_LoadedFrames.end();
	while (it != itEnd)
	{
		freeFrame(*it);
		++it;
	}

	m_Header = {};
	m_pStream = nullptr;
	memset(m_Descriptor, 0, sizeof(m_Descriptor));
	m_SeekTable.clear();

	m_LoadedFrames.clear();
	m_IsFrameLoaded.clear();
	m_FramesOffset = 0;
}

void QuantisationDecompressor::prefetch(size_t frameIndex, size_t range)
{
	const size_t frameSeekIndex = getSeekTableIndex(frameIndex);
	const size_t startFrame = frameSeekIndex * m_Header.FrameSeekWindowCount;
	const size_t frameToLoad = (frameIndex + range) - startFrame;

	// Load the frames from the seek window until the end of the range specified.
	m_pStream->seek(getSeekTableOffset(frameIndex), Stream::SeekOrigin::Begin);
	for (size_t iFrame = startFrame; iFrame < startFrame + frameToLoad; ++iFrame)
	{
		loadFrame(iFrame);

		//! TODO: Start an async job to preload after the current frame?...
	}
}

bool QuantisationDecompressor::getData(size_t frameIndex, float& time, GeomCacheData& data)
{
	time = getFrameTime(frameIndex);
	if (std::isfinite(time))
	{
		return getData(time, data);
	}

	return false;
}

bool QuantisationDecompressor::getData(float time, GeomCacheData& data)
{
	const auto it = std::lower_bound(
		  m_LoadedFrames.begin()
		, m_LoadedFrames.end()
		, FrameDataType { time, GeomCacheData{} }
		, [](const FrameDataType& lhs, const FrameDataType& rhs) -> bool {
			return lhs.Time < rhs.Time;
		}
	);

	if (it != m_LoadedFrames.end())
	{
		data = it->Data;
		return true;
	}

	return false;
}

size_t QuantisationDecompressor::getConstantDataStringSize() const
{
	if (!m_ConstantData.empty())
	{
		return InputGeomCacheConstantData::getStringCountFromData(m_ConstantData.data(), m_ConstantData.size());
	}
	return 0;
}

const char* QuantisationDecompressor::getConstantDataString(size_t index) const
{
	if (!m_ConstantData.empty())
	{
		return InputGeomCacheConstantData::getStringFromData(m_ConstantData.data(), m_ConstantData.size(), index);
	}
	return nullptr;
}

void QuantisationDecompressor::loadFrame(size_t frameIndex)
{
	quantisation_compression::FrameHeader frameHeader{};
	m_pStream->read(frameHeader);

	FrameDataType frameData{};
	frameData.Time = m_FrameTimeTable[frameIndex];
	frameData.Data.indexCount = frameHeader.IndexCount;
	frameData.Data.vertexCount = frameHeader.VertexCount;

	frameData.Data.meshCount = m_pStream->read<uint64_t>();
	frameData.Data.meshes = new GeomMesh[frameData.Data.meshCount];
	for (size_t iMesh = 0; iMesh < frameData.Data.meshCount; ++iMesh)
	{
		m_pStream->read(frameData.Data.meshes[iMesh]);
	}

    frameData.Data.submeshCount = m_pStream->read<uint64_t>();
    frameData.Data.submeshes = new GeomSubmesh[frameData.Data.submeshCount];
    for (size_t iSubmesh = 0; iSubmesh < frameData.Data.submeshCount; ++iSubmesh)
    {
        m_pStream->read(frameData.Data.submeshes[iSubmesh]);
    }

	if (frameHeader.IndexCount > 0)
	{
		const size_t dataSize = sizeof(int) * frameHeader.IndexCount;

		void *indices = malloc(dataSize);
		if (indices != nullptr)
		{
			m_pStream->read(indices, dataSize);
		}
		else
		{
			m_pStream->seek(dataSize, Stream::SeekOrigin::Current);
		}

		frameData.Data.indices = indices;
	}

	if (frameHeader.VertexCount > 0)
	{
		const size_t attributeCount = getAttributeCount(m_Descriptor);
		frameData.Data.vertices = new void*[attributeCount];

		AABB verticesAABB{};
		m_pStream->read(verticesAABB);

		for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
		{
			size_t dataSize = getSizeOfDataFormat(m_Descriptor[iAttribute].format) * frameData.Data.vertexCount;
			if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_POINTS) == 0)
			{
				dataSize = getSizeOfDataFormat(DataFormat::UNorm16x3) * frameData.Data.vertexCount;
			}
			else if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_NORMALS) == 0)
			{
				dataSize = getSizeOfDataFormat(DataFormat::UNorm16x2) * frameData.Data.vertexCount;
			}
			else if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_TANGENTS) == 0)
			{
				dataSize = getSizeOfDataFormat(DataFormat::UNorm16x2) * frameData.Data.vertexCount;
			}
			else if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_UV0) == 0
				|| _stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_UV1) == 0)
			{
				dataSize = getSizeOfDataFormat(DataFormat::UNorm16x2) * frameData.Data.vertexCount;
			}

			void *vertexData = malloc(dataSize);
			if (vertexData != nullptr)
			{
				m_pStream->read(vertexData, dataSize);
			}
			else
			{
				m_pStream->seek(dataSize, Stream::SeekOrigin::Current);
			}

			if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_POINTS) == 0)
			{
				unorm16x3* packedVertices = static_cast<unorm16x3*>(vertexData);
				float3 *unpackedVertices = static_cast<float3*>(malloc(getSizeOfDataFormat(DataFormat::Float3) * frameData.Data.vertexCount));

				for (size_t iVertex = 0; iVertex < frameData.Data.vertexCount; ++iVertex)
				{
					unpackedVertices[iVertex] = UnpackPoint(verticesAABB, packedVertices[iVertex]);
				}

				free(vertexData);
				vertexData = unpackedVertices;
			}
			else if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_NORMALS) == 0)
			{
				unorm16x2* packedNormals = static_cast<unorm16x2*>(vertexData);
				float3 *unpackedNormals = static_cast<float3*>(malloc(getSizeOfDataFormat(DataFormat::Float3) * frameData.Data.vertexCount));

				for (size_t iVertex = 0; iVertex < frameData.Data.vertexCount; ++iVertex)
				{
					float2 n; n[0] = packedNormals[iVertex][0].to_float(); n[1] = packedNormals[iVertex][1].to_float();
					unpackedNormals[iVertex] = OctDecode(n);
				}

				free(vertexData);
				vertexData = unpackedNormals;
			}
			else if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_TANGENTS) == 0)
			{
				unorm16x2* packedTangents = static_cast<unorm16x2*>(vertexData);
				float4 *unpackedTangents = static_cast<float4*>(malloc(getSizeOfDataFormat(DataFormat::Float3) * frameData.Data.vertexCount));

				for (size_t iVertex = 0; iVertex < frameData.Data.vertexCount; ++iVertex)
				{
					float2 packed;
					packed[0] = packedTangents[iVertex][0].to_float();
					packed[1] = packedTangents[iVertex][1].to_float();

					float3 t = OctDecode(packed);
					unpackedTangents[iVertex][0] = t[0];
					unpackedTangents[iVertex][1] = t[1];
					unpackedTangents[iVertex][2] = t[2];
					unpackedTangents[iVertex][3] = 1.0f;
				}

				free(vertexData);
				vertexData = unpackedTangents;
			}
			else if (_stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_UV0) == 0
				|| _stricmp(m_Descriptor[iAttribute].semantic, nvcSEMANTIC_UV1) == 0)
			{
				unorm16x2* packedUVs = static_cast<unorm16x2*>(vertexData);
				float2 *unpackedUVs = static_cast<float2*>(malloc(getSizeOfDataFormat(DataFormat::Float2) * frameData.Data.vertexCount));

				for (size_t iVertex = 0; iVertex < frameData.Data.vertexCount; ++iVertex)
				{
					unpackedUVs[iVertex][0] = packedUVs[iVertex][0].to_float();
					unpackedUVs[iVertex][1] = packedUVs[iVertex][1].to_float();
				}

				free(vertexData);
				vertexData = unpackedUVs;
			}

			frameData.Data.vertices[iAttribute] = vertexData;
		}
	}

	if (!insertLoadedData(frameIndex, frameData))
	{
		freeFrame(frameData);
	}
}

void QuantisationDecompressor::freeFrame(FrameDataType& data) const
{
	freeGeomCacheData(data.Data, getAttributeCount(m_Descriptor));
	data.Data = GeomCacheData{};
}

bool QuantisationDecompressor::insertLoadedData(size_t frameIndex, const FrameDataType& data)
{
	float time = m_FrameTimeTable[frameIndex];

	const auto it = std::find_if(m_LoadedFrames.begin(), m_LoadedFrames.end(),
		[time](const FrameDataType& d)
	{
		return d.Time == time;
	});

	if (it == m_LoadedFrames.end())
	{
		// Insert sorted.
		const auto itInsert = std::lower_bound(m_LoadedFrames.begin(), m_LoadedFrames.end(), data,
			[](const FrameDataType& lhs, const FrameDataType& rhs)
		{
			return lhs.Time < rhs.Time;
		});

		m_LoadedFrames.insert(itInsert, data);
		m_IsFrameLoaded[frameIndex] = true;

		return true;
	}

	return false;
}

} //namespace nvc
