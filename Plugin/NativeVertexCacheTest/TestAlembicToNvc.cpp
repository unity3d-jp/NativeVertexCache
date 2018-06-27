#include "Plugin/PrecompiledHeader.h"
#include "Plugin/AlembicToGeomCache/AlembicToGeomCache.h"
#include "Plugin/Stream/FileStream.h"
#include "Plugin/Compression/NullCompressor.h"
#include "Plugin/InputGeomCache.h"
#include "Plugin/GeomCacheData.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"
#include "Plugin/Types.h"

namespace {
	using namespace nvc;
	template<typename T>
void convertDataArrayToFloat2(float2* dst, const T* src, size_t numberOfElements) {
	for(size_t i = 0; i < numberOfElements; ++i) {
		dst[i] = to_float(src[i]);
	}
}

template<typename T>
void convertDataArrayToFloat3(float3* dst, const T* src, size_t numberOfElements) {
	for(size_t i = 0; i < numberOfElements; ++i) {
		dst[i] = to_float(src[i]);
	}
}

template<typename T>
void convertDataArrayToFloat4(float4* dst, const T* src, size_t numberOfElements) {
	for(size_t i = 0; i < numberOfElements; ++i) {
		dst[i] = to_float(src[i]);
	}
}

void convertDataArrayToFloat2(float2* dst, const void* src, size_t numberOfElements, DataFormat dataFormat) {
	switch(dataFormat) {
	default:		assert(false && "DataFormat must have 2 components");		break;
//	case DataFormat::Int2:		convertDataArrayToFloat2(dst, static_cast<const int2*>     (src), numberOfElements);	break;
	case DataFormat::Float2:	convertDataArrayToFloat2(dst, static_cast<const float2*>   (src), numberOfElements);	break;
	case DataFormat::Half2:		convertDataArrayToFloat2(dst, static_cast<const half2*>    (src), numberOfElements);	break;
	case DataFormat::SNorm16x2:	convertDataArrayToFloat2(dst, static_cast<const snorm16x2*>(src), numberOfElements);	break;
	case DataFormat::UNorm16x2:	convertDataArrayToFloat2(dst, static_cast<const unorm16x2*>(src), numberOfElements);	break;
	}
}

void convertDataArrayToFloat3(float3* dst, const void* src, size_t numberOfElements, DataFormat dataFormat) {
	switch(dataFormat) {
	default:		assert(false && "DataFormat must have 3 components");		break;
//	case DataFormat::Int3:		convertDataArrayToFloat3(dst, static_cast<const int3*>     (src), numberOfElements);	break;
	case DataFormat::Float3:	convertDataArrayToFloat3(dst, static_cast<const float3*>   (src), numberOfElements);	break;
	case DataFormat::Half3:		convertDataArrayToFloat3(dst, static_cast<const half3*>    (src), numberOfElements);	break;
	case DataFormat::SNorm16x3:	convertDataArrayToFloat3(dst, static_cast<const snorm16x3*>(src), numberOfElements);	break;
	case DataFormat::UNorm16x3:	convertDataArrayToFloat3(dst, static_cast<const unorm16x3*>(src), numberOfElements);	break;
	}
}

void convertDataArrayToFloat4(float4* dst, const void* src, size_t numberOfElements, DataFormat dataFormat) {
	switch(dataFormat) {
	default:		assert(false && "DataFormat must have 4 components");		break;
//	case DataFormat::Int4:		convertDataArrayToFloat4(dst, static_cast<const int4*>     (src), numberOfElements);	break;
	case DataFormat::Float4:	convertDataArrayToFloat4(dst, static_cast<const float4*>   (src), numberOfElements);	break;
	case DataFormat::Half4:		convertDataArrayToFloat4(dst, static_cast<const half4*>    (src), numberOfElements);	break;
	case DataFormat::SNorm16x4:	convertDataArrayToFloat4(dst, static_cast<const snorm16x4*>(src), numberOfElements);	break;
	case DataFormat::UNorm16x4:	convertDataArrayToFloat4(dst, static_cast<const unorm16x4*>(src), numberOfElements);	break;
	}
}
} // namespace

static void dump(const nvc::InputGeomCache& igc) {
	using namespace nvc;
	using namespace nvcabc;
	const auto nFrame = igc.getDataCount();
	GeomCacheDesc geomCacheDescs[GEOM_CACHE_MAX_DESCRIPTOR_COUNT + 1] {};
	igc.getDesc(&geomCacheDescs[0]);

//	const int descIndex_points   = getAttributeIndex(geomCacheDescs, SEMANTIC_POINTS  );
//	const int descIndex_normals  = getAttributeIndex(geomCacheDescs, SEMANTIC_NORMALS );
//	const int descIndex_tangents = getAttributeIndex(geomCacheDescs, SEMANTIC_TANGENTS);
//	const int descIndex_uv0      = getAttributeIndex(geomCacheDescs, SEMANTIC_UV0     );
//	const int descIndex_colors   = getAttributeIndex(geomCacheDescs, SEMANTIC_COLORS  );
	const int descIndex_points   = getAttributeIndex(geomCacheDescs, nvcSemantic_Points  );
	const int descIndex_normals  = getAttributeIndex(geomCacheDescs, nvcSemantic_Normals );
	const int descIndex_tangents = getAttributeIndex(geomCacheDescs, nvcSemantic_Tangents);
	const int descIndex_uv0      = getAttributeIndex(geomCacheDescs, nvcSemantic_UV0     );
	const int descIndex_colors   = getAttributeIndex(geomCacheDescs, nvcSemantic_Colors  );

	printf("GeomCacheDesc.descIndex_points   = %d\n", descIndex_points   );
	printf("GeomCacheDesc.descIndex_normals  = %d\n", descIndex_normals  );
	printf("GeomCacheDesc.descIndex_tangents = %d\n", descIndex_tangents );
	printf("GeomCacheDesc.descIndex_uv0      = %d\n", descIndex_uv0      );
	printf("GeomCacheDesc.descIndex_colors   = %d\n", descIndex_colors   );

	printf("nFrame(%zd)\n", nFrame);
	for(size_t iFrame = 0; iFrame < nFrame; ++iFrame) {
		printf("frame(%zd), ", iFrame);
		float frameTime = 0.0f;
		GeomCacheData geomCacheData {};
		igc.getData(iFrame, frameTime, &geomCacheData);

		printf("time(%5.3f)", frameTime);
		printf("\n");

		const auto nIndex = geomCacheData.indexCount;
		printf("index[%zd]={", nIndex);
		const int32_t* pIndex = static_cast<const int32_t*>(geomCacheData.indices);
		for(size_t iIndex = 0; iIndex < nIndex; ++iIndex) {
			const int32_t index = pIndex[iIndex];
			printf("%d, ", index);
		}
		printf("}\n");

		if(descIndex_points >= 0 && geomCacheData.vertices != nullptr) {
			std::vector<float3> points(geomCacheData.vertexCount);
			const auto* p = geomCacheData.vertices[descIndex_points];
			if(p != nullptr) {
				convertDataArrayToFloat3(
					  points.data()
					, p
					, points.size()
					, geomCacheDescs[descIndex_points].format
				);
				printf("points[%zd]={", points.size());
				for(const auto& v : points) {
					printf("{%+5.3f,%+5.3f,%+5.3f}, ", v[0], v[1], v[2]);
				}
				printf("}\n");
			}
		}

		printf("\n");
	}
}

void RunTest_AlembicToNvc()
{
    using namespace nvc;
    using namespace nvcabc;

    const char* abcFilename = "../../../Data/Cloth-1frame.abc";
    const char* nvcFilename = "../../../Data/TestOutput/Cloth-1frame.nvc";

    assert(IsFileExist(abcFilename));
    RemoveFile(nvcFilename);

    AlembicImportOptions opt;
    AlembicGeometries abcGeoms;

    const auto abcToGcResult = AlembicToGeomCache(abcFilename, opt, abcGeoms);
    assert(abcToGcResult);

	dump(*abcGeoms.geometry);

    FileStream fs { nvcFilename, FileStream::OpenModes::Random_ReadWrite };
    NullCompressor nc {};
    nc.compress(*abcGeoms.geometry, &fs);
}
