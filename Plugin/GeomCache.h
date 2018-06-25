#pragma once

enum DataFormat
{
	Unknown,
	Int,
	Float3,
	SNorm16x3,
	UNorm16x2,
	Half4,
	// ...
};

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


class GeomCache
{
public:
	// last element of desc must be GEOM_CACHE_DESCRIPTOR_END (null terminator)
	GeomCache(const GeomCacheDesc *desc);
	void addData(float time, const GeomCacheData *data);

	void getDesc(GeomCacheDesc *desc) const;
	// GeomCacheData::data can be nullptr. in that case, only count will be filled.
	void getData(float time, GeomCacheData *data);
};
