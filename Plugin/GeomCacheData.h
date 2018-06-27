#pragma once

namespace nvc {

enum class DataFormat : uint32_t
{
	Unknown,
	Int,
	Int2,
	Int3,
	Int4,
	Float,
	Float2,
	Float3,
	Float4,
	Half,
	Half2,
	Half3,
	Half4,
	SNorm16,
	SNorm16x2,
	SNorm16x3,
	SNorm16x4,
	UNorm16,
	UNorm16x2,
	UNorm16x3,
	UNorm16x4,
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
	const void* indices;
	size_t indexCount;

	const void **vertices;
	size_t vertexCount;
};
void freeGeomCacheData(GeomCacheData& cacheData, size_t attributeCount);

size_t getSizeOfDataFormat(DataFormat dataFormat);
size_t getAttributeCount(const GeomCacheDesc* desc);

} // namespace nvc
