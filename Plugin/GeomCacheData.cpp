//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "GeomCacheData.h"

//! Project Includes.
#include "Types.h"

namespace nvc 
{

void freeGeomCacheData(GeomCacheData& cacheData, size_t attributeCount)
{
	void *data = const_cast<void*>(cacheData.indices);
	free(data);

	for (size_t iAttribute = 0; iAttribute < attributeCount; ++iAttribute)
	{
		void *vertexData = const_cast<void*>(cacheData.vertices[iAttribute]);
		free(vertexData);
	}

	delete[] cacheData.vertices;
}

size_t getSizeOfDataFormat(DataFormat dataFormat)
{
	switch (dataFormat)
	{
	default:
	case DataFormat::Unknown:	return 0;
	case DataFormat::Int:		return 1 * sizeof(int32_t);
	case DataFormat::Int2:		return 2 * sizeof(int32_t);
	case DataFormat::Int3:		return 3 * sizeof(int32_t);
	case DataFormat::Int4:		return 4 * sizeof(int32_t);
	case DataFormat::Float:		return 1 * sizeof(float);
	case DataFormat::Float2:	return 2 * sizeof(float);
	case DataFormat::Float3:	return 3 * sizeof(float);
	case DataFormat::Float4:	return 4 * sizeof(float);
	case DataFormat::Half:		return 1 * sizeof(half);
	case DataFormat::Half2:		return 2 * sizeof(half);
	case DataFormat::Half3:		return 3 * sizeof(half);
	case DataFormat::Half4:		return 4 * sizeof(half);
	case DataFormat::SNorm16:	return 1 * sizeof(snorm16);
	case DataFormat::SNorm16x2:	return 2 * sizeof(snorm16);
	case DataFormat::SNorm16x3:	return 3 * sizeof(snorm16);
	case DataFormat::SNorm16x4:	return 4 * sizeof(snorm16);
	case DataFormat::UNorm16:	return 1 * sizeof(unorm16);
	case DataFormat::UNorm16x2:	return 2 * sizeof(unorm16);
	case DataFormat::UNorm16x3:	return 3 * sizeof(unorm16);
	case DataFormat::UNorm16x4:	return 4 * sizeof(unorm16);
	}
}

size_t getAttributeCount(const GeomCacheDesc* desc)
{
	size_t count = 0;

	const GeomCacheDesc *currentDesc = desc;
	while (currentDesc->semantic != nullptr)
	{
		++currentDesc;
		++count;
	}

	return count;
}
const char* SEMANTIC_POINTS = "points";
const char* SEMANTIC_VELOCITIES = "velocities";
const char* SEMANTIC_NORMALS = "normals";
const char* SEMANTIC_TANGENTS = "tangents";
const char* SEMANTIC_UV0 = "uv0";
const char* SEMANTIC_UV1 = "uv1";
const char* SEMANTIC_COLORS = "colors";
const char* SEMANTIC_VERTEXID = "vertexid";
const char* SEMANTIC_MESHID = "meshid";

int getAttributeIndex(const GeomCacheDesc *desc, const char *semantic)
{
	const size_t itemCount = getAttributeCount(desc);
	for (size_t iItem = 0; iItem < itemCount; ++iItem)
	{
		if (_stricmp(desc[iItem].semantic, semantic) != 0)
		{
			return static_cast<int>(iItem);
		}
	}

	return -1;
}

bool hasAttribute(const GeomCacheDesc *desc, const char *semantic)
{
	return getAttributeIndex(desc, semantic) > 0;
}

} // namespace nvc
