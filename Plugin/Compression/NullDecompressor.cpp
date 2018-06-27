//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "NullDecompressor.h"

//! Project Includes.
#include "Plugin/Stream/Stream.h"
#include "NullTypes.h"

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

	for (uint32_t iElement = 0; iElement < m_Header.VertexAttributeCount; ++iElement)
	{
		m_pStream->read(m_Descriptor[iElement]);
	}

	m_FramesOffset = m_pStream->getPosition();

	// Read the seek table.
	const size_t seekTableSize = (m_Header.FrameCount + m_Header.FrameSeekWindowCount - 1) / m_Header.FrameSeekWindowCount + 1;
	for (uint64_t iEntry = 0; iEntry < seekTableSize; ++iEntry)
	{
		m_SeekTable.push_back(m_pStream->read<uint64_t>());
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
	const size_t attributeCount = getAttributeCount(m_Descriptor);

	auto it = m_LoadedFrames.begin();
	const auto itEnd = m_LoadedFrames.end();
	while (it != itEnd)
	{
		freeGeomCacheData(it->second, attributeCount);
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
	const auto it = std::find_if(m_LoadedFrames.begin(), m_LoadedFrames.end(),
		[time](const FrameDataType& d)
	{
		return d.first == time;
	});

	if (it == m_LoadedFrames.end())
	{
		data = it->second;
		return true;
	}

	return false;
}

void NullDecompressor::loadFrame(size_t frameIndex)
{
	null_compression::FrameHeader frameHeader{};
	m_pStream->read(frameHeader);

	GeomCacheData cacheData = {};
	cacheData.indexCount = frameHeader.IndexCount;
	cacheData.vertexCount = frameHeader.VertexCount;

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

		cacheData.indices = indices;
	}

	if (frameHeader.VertexCount > 0)
	{
		const size_t attributeCount = getAttributeCount(m_Descriptor);
		cacheData.vertices = new const void*[attributeCount];

		for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
		{
			const size_t dataSize = getSizeOfDataFormat(m_Descriptor[iAttribute].format) * cacheData.vertexCount;

			void *vertexData = malloc(dataSize);
			if (vertexData != nullptr)
			{
				m_pStream->write(vertexData, dataSize);
			}
			else
			{
				m_pStream->seek(dataSize, Stream::SeekOrigin::Current);
			}

			cacheData.vertices[iAttribute] = vertexData;
		}
	}

	insertLoadedData(frameIndex, cacheData);
}

void NullDecompressor::insertLoadedData(size_t frameIndex, const GeomCacheData& data)
{
	float time = m_FrameTimeTable[frameIndex];

	const auto it = std::find_if(m_LoadedFrames.begin(), m_LoadedFrames.end(),
		[time](const FrameDataType& d)
	{
		return d.first == time;
	});

	if (it == m_LoadedFrames.end())
	{
		// Insert sorted.
		const auto dataToInsert = std::make_pair(time, data);
		const auto itInsert = std::lower_bound(m_LoadedFrames.begin(), m_LoadedFrames.end(), dataToInsert,
			[](const FrameDataType& lhs, const FrameDataType& rhs)
		{
			return lhs.first < rhs.first;
		});

		m_LoadedFrames.insert(itInsert, dataToInsert);
		m_IsFrameLoaded[frameIndex] = true;
	}
}

} //namespace nvc
