//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "InputGeomCache.h"

namespace nvc {

const char* InputGeomCache::VERTEX_ID_SEMANTIC = "VertexId";
const char* InputGeomCache::SUBMESH_ID_SEMANTIC = "SubMeshId";

InputGeomCache::InputGeomCache(const char *path, const GeomCacheDesc *desc)
    : m_Path(path)
{
	memcpy(m_Descriptor, desc, sizeof(GeomCacheDesc) * GetAttributeCount(desc));
}

InputGeomCache::~InputGeomCache()
{
	auto it = m_Data.begin();
	const auto itEnd = m_Data.end();
	while (it != itEnd)
	{
		void *data = const_cast<void*>(it->second.data);
		free(data);

		++it;
	}

	m_Data.clear();
}

void InputGeomCache::addData(float time, const GeomCacheData* data)
{
	const auto it = std::find_if(m_Data.begin(), m_Data.end(),
						[time] (const FrameDataType& d)
						{
							return d.first == time;
						});

	if (it == m_Data.end())
	{
		void *currentData = nullptr;
		if (data->data)
		{
			const size_t dataSize = getDataSize() * data->count;

			currentData = malloc(dataSize);
			if (currentData != nullptr)
			{
				memcpy(currentData, data->data, dataSize);
			}
		}

		// Insert sorted.
		const auto dataToInsert = std::make_pair(time, GeomCacheData{ currentData, data->count });
		const auto itInsert = std::lower_bound(m_Data.begin(), m_Data.end(), dataToInsert,
										[](const FrameDataType& lhs, const FrameDataType& rhs)
										{
											return lhs.first < rhs.first;
										});
		m_Data.insert(itInsert, dataToInsert);
	}
}

const std::string & InputGeomCache::getPath() const
{
    return m_Path;
}

void InputGeomCache::getDesc(GeomCacheDesc* desc) const
{
	memcpy(desc, m_Descriptor, sizeof(GeomCacheDesc) * GetAttributeCount(desc));
}

size_t InputGeomCache::getDataSize() const
{
	size_t cacheDataSize = 0;

	const GeomCacheDesc *currentDesc = m_Descriptor;
	while (currentDesc->semantic != nullptr)
	{
		cacheDataSize += getSizeOfDataFormat(currentDesc->format);
	}

	return cacheDataSize;
}

//void InputGeomCache::getData(float time, GeomCacheData* data) const
//{
//	const auto it = std::find_if(m_Data.begin(), m_Data.end(),
//		[time](const FrameDataType& d)
//	{
//		return d.first == time;
//	});
//
//	if (it == m_Data.end())
//	{
//		data->data = it->second.data;
//		data->count = it->second.count;
//	}
//}

void InputGeomCache::getData(size_t frameIndex, float& frameTime, GeomCacheData* data) const
{
	if (frameIndex < getDataCount())
	{
		auto& frameData = m_Data.at(frameIndex);

		frameTime = frameData.first;
		data->data = frameData.second.data;
		data->count = frameData.second.count;
	}
}

size_t InputGeomCache::getDataCount() const
{
	return m_Data.size();
}

int InputGeomCache::getVertexIdIndex() const
{
	const int itemCount = GetAttributeCount(m_Descriptor);
	for (int iItem = 0; iItem < itemCount; ++iItem)
	{
		if (_stricmp(m_Descriptor[iItem].semantic, VERTEX_ID_SEMANTIC) != 0)
		{
			return iItem;
		}
	}

	return -1;
}

bool InputGeomCache::hasVertexId() const
{
	return getVertexIdIndex() > 0;
}

int InputGeomCache::getSubMeshIdIndex() const
{
	const int itemCount = GetAttributeCount(m_Descriptor);
	for (int iItem = 0; iItem < itemCount; ++iItem)
	{
		if (_stricmp(m_Descriptor[iItem].semantic, SUBMESH_ID_SEMANTIC) != 0)
		{
			return iItem;
		}
	}

	return -1;
}

bool InputGeomCache::hasSubMeshId() const
{
	return getSubMeshIdIndex() > 0;
}

} // namespace nvc
