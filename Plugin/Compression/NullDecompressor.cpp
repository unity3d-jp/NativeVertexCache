//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "NullDecompressor.h"

//! Project Includes.
#include "Plugin/Stream/Stream.h"
#include "Plugin/InputGeomCache.h"

namespace nvc
{

NullDecompressor::~NullDecompressor()
{
	close();
}

void NullDecompressor::open(Stream* pStream)
{
	close();

	m_pStream = pStream;

	// Read the file header.
	m_pStream->read(m_Header);

	// Read the descriptor.
	for (uint32_t iElement = 0; iElement < m_Header.VertexAttributeCount; ++iElement)
	{
		m_pStream->read(m_Semantics[iElement], sizeof(char) * null_compression::SEMANTIC_STRING_LENGTH);

		uint32_t format = 0;
		m_pStream->read(format);

		m_Descriptor[iElement].semantic = m_Semantics[iElement];
		m_Descriptor[iElement].format = static_cast<DataFormat>(format);
	}

	m_FramesOffset = m_pStream->getPosition();

	// Read the seek table.
	const size_t seekTableSize = (m_Header.FrameCount + m_Header.FrameSeekWindowCount - 1) / m_Header.FrameSeekWindowCount;
	for (uint64_t iEntry = 0; iEntry < seekTableSize; ++iEntry)
	{
		m_SeekTable.push_back(m_pStream->read<uint64_t>());
	}

	// Read constant data
	if(m_Header.ConstantDataSize > 0) {
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

void NullDecompressor::close()
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

void NullDecompressor::prefetch(size_t frameIndex, size_t range)
{
	const size_t frameSeekIndex = getSeekTableIndex(frameIndex);
	const size_t startFrame = frameSeekIndex * m_Header.FrameSeekWindowCount;
	const size_t frameToLoad = (frameIndex + range) - startFrame;

	// Load the frames from the seek window until the end of the range specified.
	m_pStream->seek(getSeekTableOffset(frameIndex), Stream::SeekOrigin::Begin);
	for (size_t iFrame = 0; iFrame < frameToLoad; ++iFrame)
	{
		loadFrame(iFrame);

		//! TODO: Start an async job to preload after the current frame?...
	}
}

bool NullDecompressor::getData(size_t frameIndex, float& time, GeomCacheData& data)
{
	time = getFrameTime(frameIndex);
	if (std::isfinite(time))
	{
		return getData(time, data);
	}

	return false;
}

bool NullDecompressor::getData(float time, GeomCacheData& data)
{
//	const auto it = std::find_if(m_LoadedFrames.begin(), m_LoadedFrames.end(),
//		[time](const FrameDataType& d)
//	{
//		return d.Time == time;
//	});
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

size_t NullDecompressor::getConstantDataStringSize() const {
	if(m_ConstantData.size() != 0) {
		return InputGeomCacheConstantData::getStringCountFromData(m_ConstantData.data(), m_ConstantData.size());
	}
	return 0;
}

const char* NullDecompressor::getConstantDataString(size_t index) const {
	if(m_ConstantData.size() != 0) {
		return InputGeomCacheConstantData::getStringFromData(m_ConstantData.data(), m_ConstantData.size(), index);
	}
	return nullptr;
}

void NullDecompressor::loadFrame(size_t frameIndex)
{
	null_compression::FrameHeader frameHeader{};
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

		for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
		{
			const size_t dataSize = getSizeOfDataFormat(m_Descriptor[iAttribute].format) * frameData.Data.vertexCount;

			void *vertexData = malloc(dataSize);
			if (vertexData != nullptr)
			{
				m_pStream->read(vertexData, dataSize);
			}
			else
			{
				m_pStream->seek(dataSize, Stream::SeekOrigin::Current);
			}

			frameData.Data.vertices[iAttribute] = vertexData;
		}
	}

	if (!insertLoadedData(frameIndex, frameData))
	{
		freeFrame(frameData);
	}
}

void NullDecompressor::freeFrame(FrameDataType& data) const
{
	freeGeomCacheData(data.Data, getAttributeCount(m_Descriptor));
	data.Data = GeomCacheData{};
}

bool NullDecompressor::insertLoadedData(size_t frameIndex, const FrameDataType& data)
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
