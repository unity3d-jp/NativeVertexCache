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
	m_Header = {};
	m_pStream = nullptr;
	memset(m_Descriptor, 0, sizeof(m_Descriptor));
	m_SeekTable.clear();

	auto it = m_LoadedFrames.begin();
	const auto itEnd = m_LoadedFrames.end();
	while (it != itEnd)
	{
		void *data = const_cast<void*>(it->second.data);
		free(data);
		++it;
	}

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

bool NullDecompressor::getData(size_t /*frameIndex*/, float& /*time*/, const GeomCacheData& /*data*/)
{
	return false;
}

bool NullDecompressor::getData(float /*time*/, const GeomCacheData& /*data*/)
{
	return false;
}

void NullDecompressor::addLoadedData(size_t frameIndex, const GeomCacheData& data)
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

size_t NullDecompressor::getDataSize() const
{
	size_t cacheDataSize = 0;

	const GeomCacheDesc *currentDesc = m_Descriptor;
	while (currentDesc->semantic != nullptr)
	{
		cacheDataSize += getSizeOfDataFormat(currentDesc->format);
	}

	return cacheDataSize;
}

void NullDecompressor::loadFrame(size_t frameIndex)
{
	null_compression::FrameHeader frameHeader{};
	m_pStream->read(frameHeader);

	const size_t cacheDataSize = getDataSize() * frameHeader.VertexCount;
	void* currentData = malloc(cacheDataSize);
	if (currentData != nullptr)
	{
		const size_t readCount = m_pStream->read(currentData, cacheDataSize);
		if (readCount > 0)
		{
			addLoadedData(frameIndex, GeomCacheData { currentData, frameHeader.VertexCount });
		}
		else
		{
			free(currentData);
		}
	}
}

} //namespace nvc
