#pragma once
#include <map>
#include "DataFormat.h"

using DataFormat = nvc::DataFormat;	// TODO : Should be removed.

#define GEOM_CACHE_MAX_DESCRIPTOR_COUNT        (8)
struct GeomCacheDesc
{
	const char *semantic;
	DataFormat format;
};
#define GEOM_CACHE_DESCRIPTOR_END            { nullptr, DataFormat::Unknown }

struct GeomCacheData
{
	const void *data;
	size_t count;
};


class InputGeomCache final
{
private:
	GeomCacheDesc m_Descriptor[GEOM_CACHE_MAX_DESCRIPTOR_COUNT] = {};

	std::map<float, GeomCacheData> m_Data;

public:
	// last element of desc must be GEOM_CACHE_DESCRIPTOR_END (null terminator)
	InputGeomCache(const GeomCacheDesc *desc);
	~InputGeomCache();

	void addData(float time, const GeomCacheData *data);

	void getDesc(GeomCacheDesc *desc) const;
	size_t getDataSize() const;
	// GeomCacheData::data can be nullptr. in that case, only count will be filled.
	void getData(float time, GeomCacheData* data);

	//...
	InputGeomCache(const InputGeomCache&) = delete;
	InputGeomCache(InputGeomCache&&) = delete;
	InputGeomCache& operator=(const InputGeomCache&) = delete;
	InputGeomCache& operator=(InputGeomCache&&) = delete;
};
