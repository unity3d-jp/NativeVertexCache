#pragma once

// note: these are referenced by other plugins. in this case macro is easier to maintain than constants.
#define nvcSEMANTIC_POINTS     "points"
#define nvcSEMANTIC_VELOCITIES "velocities"
#define nvcSEMANTIC_NORMALS    "normals"
#define nvcSEMANTIC_TANGENTS   "tangents"
#define nvcSEMANTIC_UV0        "uv0"
#define nvcSEMANTIC_UV1        "uv1"
#define nvcSEMANTIC_COLORS     "colors"
#define nvcSEMANTIC_VERTEXID   "vertexid"


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

enum class Topology : uint32_t
{
    Points,
    Lines,
    Triangles,
    Quads,
};


#define GEOM_CACHE_MAX_DESCRIPTOR_COUNT        (8)
struct GeomCacheDesc
{
	const char *semantic;
	DataFormat format;
};
#define GEOM_CACHE_DESCRIPTOR_END            { nullptr, DataFormat::Unknown }

struct GeomSubmesh
{
    uint32_t indexOffset, indexCount;
    Topology topology;
};

struct GeomMesh
{
    uint32_t vertexOffset, vertexCount;
    uint32_t submeshOffset, submeshCount;
};

struct GeomCacheData
{
	void* indices;
	size_t indexCount;

    void **vertices;
	size_t vertexCount;

    GeomMesh *meshes;
    size_t meshCount;

    GeomSubmesh *submeshes;
    size_t submeshCount;
};
void freeGeomCacheData(GeomCacheData& cacheData, size_t attributeCount);

size_t getSizeOfDataFormat(DataFormat dataFormat);
size_t getAttributeCount(const GeomCacheDesc* desc);

int getAttributeIndex(const GeomCacheDesc *desc, const char *semantic);
bool hasAttribute(const GeomCacheDesc *desc, const char *semantic);

} // namespace nvc
