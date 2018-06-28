//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "InputGeomCache.h"
#include "Stream/Stream.h"
#include "Stream/MemoryStream.h"

namespace nvc {

InputGeomCache::InputGeomCache(const GeomCacheDesc *desc, const InputGeomCacheConstantData *constantData)
{
	memcpy(m_Descriptor, desc, sizeof(GeomCacheDesc) * getAttributeCount(desc));
	if(constantData != nullptr) {
		m_ConstantData.copyFrom(*constantData);
	}
}

InputGeomCache::~InputGeomCache()
{
    clearData();
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
		GeomCacheData cacheData = {};
		cacheData.indexCount = data->indexCount;
		cacheData.vertexCount = data->vertexCount;

		if (data->indices)
		{
			const size_t dataSize = sizeof(int) * data->indexCount;

			void *indices = malloc(dataSize);
			if (indices != nullptr)
			{
				memcpy(indices, data->indices, dataSize);
			}

			cacheData.indices = indices;
		}

		if (data->vertices)
		{
			const size_t attributeCount = getAttributeCount(m_Descriptor);
			cacheData.vertices = new void*[attributeCount];

			for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
			{
				const size_t dataSize = getSizeOfDataFormat(m_Descriptor[iAttribute].format) * data->vertexCount;

				void *vertexData = malloc(dataSize);
				if (vertexData != nullptr)
				{
					memcpy(vertexData, data->vertices[iAttribute], dataSize);
				}

				cacheData.vertices[iAttribute] = vertexData;
			}
		}

        cacheData.meshCount = data->meshCount;
        if (data->meshes) {
            cacheData.meshes = new GeomMesh[data->meshCount];
            std::copy(data->meshes, data->meshes + data->meshCount, cacheData.meshes);
        }

        cacheData.submeshCount = data->submeshCount;
        if (data->submeshes) {
            cacheData.submeshes = new GeomSubmesh[data->submeshCount];
            std::copy(data->submeshes, data->submeshes + data->submeshCount, cacheData.submeshes);
        }

		// Insert sorted.
		const auto dataToInsert = std::make_pair(time, cacheData);
		const auto itInsert = std::lower_bound(m_Data.begin(), m_Data.end(), dataToInsert,
										[](const FrameDataType& lhs, const FrameDataType& rhs)
										{
											return lhs.first < rhs.first;
										});
		m_Data.insert(itInsert, dataToInsert);
	}
}

void InputGeomCache::clearData()
{
    const size_t attributeCount = getAttributeCount(m_Descriptor);

    auto it = m_Data.begin();
    const auto itEnd = m_Data.end();
    while (it != itEnd)
    {
        freeGeomCacheData(it->second, attributeCount);
        ++it;
    }

    m_Data.clear();
}

void InputGeomCache::getDesc(GeomCacheDesc* desc) const
{
	memcpy(desc, m_Descriptor, sizeof(GeomCacheDesc) * getAttributeCount(m_Descriptor));
}

void InputGeomCache::getConstantData(InputGeomCacheConstantData& constantData) const {
	constantData.copyFrom(this->m_ConstantData);
}

void InputGeomCache::getData(size_t frameIndex, float& frameTime, GeomCacheData* data) const
{
	if (frameIndex < getDataCount())
	{
		auto& frameData = m_Data.at(frameIndex);

		frameTime = frameData.first;
		data->indices = frameData.second.indices;
		data->indexCount = frameData.second.indexCount;
		data->vertices = frameData.second.vertices;
		data->vertexCount = frameData.second.vertexCount;
        data->meshes = frameData.second.meshes;
        data->meshCount = frameData.second.meshCount;
        data->submeshes = frameData.second.submeshes;
        data->submeshCount = frameData.second.submeshCount;
    }
}

size_t InputGeomCache::getDataCount() const
{
	return m_Data.size();
}



size_t InputGeomCacheConstantData::getStringCount() const {
	return m_Strings.size();
}

const char* InputGeomCacheConstantData::getString(size_t stringIndex) const {
	if(stringIndex >= m_Strings.size()) {
		return nullptr;
	}
	const auto& s = m_Strings[stringIndex];
	if(s.size() == 0) {
		return nullptr;
	} else {
		return s.data();
	}
}

void InputGeomCacheConstantData::clearStrings() {
	m_Strings.clear();
}

size_t InputGeomCacheConstantData::addString(const char* text) {
	const auto index = m_Strings.size();
	const auto textLen = strlen(text) + 1;
	std::vector<char> buf(textLen);
	memcpy(buf.data(), text, buf.size());
    buf.back() = 0;
	m_Strings.push_back(buf);
	return index;
}

size_t InputGeomCacheConstantData::getSizeAsByteArray() const {
	size_t size = 0;

	const auto numStrings = static_cast<uint32_t>(m_Strings.size());
	size += sizeof(numStrings);
	for(uint32_t iString = 0; iString < numStrings; ++iString) {
		const auto& str = m_Strings[iString];
		size += sizeof(uint32_t);
		if(str.size() > 0) {
			size += str.size();
		}
	}
	return size;
}

// reconstruct from byte stream
bool InputGeomCacheConstantData::loadDataFrom(const Stream* source) {
	const auto numStrings = source->read<uint32_t>();
	m_Strings.resize(numStrings);
	for(uint32_t iString = 0; iString < numStrings; ++iString) {
		const auto strLength = source->read<uint32_t>();
		std::vector<char> buf(strLength);
		if(buf.size() > 0) {
			source->read(buf.data(), buf.size());
		}
		m_Strings[iString] = buf;
	}
	return true;
}

// represent structure as byte stream
bool InputGeomCacheConstantData::storeDataTo(Stream* destination) const {
	const auto numStrings = static_cast<uint32_t>(m_Strings.size());
	destination->write<uint32_t>(numStrings);
	for(uint32_t iString = 0; iString < numStrings; ++iString) {
		const auto& str = m_Strings[iString];
		destination->write<uint32_t>(static_cast<uint32_t>(str.size()));
		if(str.size() > 0) {
			destination->write(str.data(), str.size());
		}
	}
	return true;
}

bool InputGeomCacheConstantData::copyFrom(const InputGeomCacheConstantData& srcConstantData) {
	MemoryStream memStream(1024, true);
	srcConstantData.storeDataTo(&memStream);
    memStream.setLength(memStream.getPosition());
	memStream.seek(0, Stream::SeekOrigin::Begin);
	this->loadDataFrom(&memStream);
	return true;
}

size_t InputGeomCacheConstantData::getStringCountFromData(const void* data, size_t dataSize)
{
	if(dataSize < sizeof(uint32_t)) {
		return 0;
	}
	return * static_cast<const uint32_t*>(data);
}

const char* InputGeomCacheConstantData::getStringFromData(const void* data, size_t dataSize, size_t index)
{
	//	uint32_t	numStrings;
	//	String		strings[numStrings];
	//
	//	struct String {
	//		uint32_t	length;
	//		uint8_t		str[length];
	//	};
	const auto* p = static_cast<const uint8_t*>(data);

	const size_t numStrings = getStringCountFromData(data, dataSize);
	if(index >= numStrings) {
		return nullptr;
	}
	p += sizeof(uint32_t);

	for(size_t iIndex = 0; ; ++iIndex) {
		const uint32_t length = * (const uint32_t*) p;
		p += sizeof(uint32_t);
		if(iIndex >= index) {
			break;
		}
		p += length;
	}
	return reinterpret_cast<const char*>(p);
}

} // namespace nvc
