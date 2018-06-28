#pragma once

#include "GeomCacheData.h"

class Stream;

namespace nvc {

struct InputGeomCacheConstantData
{
private:
	std::vector<std::vector<char>> m_Strings;

public:
	static size_t getStringCountFromData(const void* data, size_t dataSize);
	static const char* getStringFromData(const void* data, size_t dataSize, size_t index);

	//	struct String {
	//		uint32_t	stringLen;
	//		uint8_t		string[stringLen];
	//	};
	//
	//	uint32_t	numStrings;
	//	String strings[numStrings];
	//
	size_t getStringCount() const;
	const char* getString(size_t stringIndex) const;
	void clearStrings();
	size_t addString(const char* text);

	size_t getSizeAsByteArray() const;
	bool loadDataFrom(const Stream* source);
	bool storeDataTo(Stream* destination) const;
	bool copyFrom(const InputGeomCacheConstantData& srcConstantData);
};

class InputGeomCache final
{
public:
private:
	using FrameDataType = std::pair<float, GeomCacheData>;

	GeomCacheDesc m_Descriptor[GEOM_CACHE_MAX_DESCRIPTOR_COUNT] = {};
	std::vector<FrameDataType> m_Data;
	InputGeomCacheConstantData m_ConstantData;

public:
	// last element of desc must be GEOM_CACHE_DESCRIPTOR_END (null terminator)
	InputGeomCache(const GeomCacheDesc *desc, const InputGeomCacheConstantData* constantData = nullptr);
	~InputGeomCache();

	void addData(float time, const GeomCacheData *data);
    void clearData();

	void getDesc(GeomCacheDesc *desc) const;
	void getConstantData(InputGeomCacheConstantData& constantData) const;
	// GeomCacheData::data can be nullptr. in that case, only count will be filled.
	void getData(size_t frameIndex, float& frameTime, GeomCacheData* data) const;
	size_t getDataCount() const;

	//...
	InputGeomCache(const InputGeomCache&) = delete;
	InputGeomCache(InputGeomCache&&) = delete;
	InputGeomCache& operator=(const InputGeomCache&) = delete;
	InputGeomCache& operator=(InputGeomCache&&) = delete;
};

} // namespace nvc
