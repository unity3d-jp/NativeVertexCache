//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "GeomCache.h"
#include "Plugin/Compression/NullDecompressor.h"

namespace nvc {

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

GeomCache::GeomCache()
{
	close();
}

GeomCache::~GeomCache()
{
}

bool GeomCache::open(const char* nvcFilename) {
	close();

	m_InputFileStream = std::unique_ptr<FileStream>(
		new FileStream(
			  nvcFilename
			, FileStream::OpenModes::Random_ReadOnly
		)
	);
	if(m_InputFileStream == nullptr || (!m_InputFileStream->canRead())) {
		return false;
	}

	m_Decompressor = std::unique_ptr<NullDecompressor>(
		new NullDecompressor()
	);
	assert(m_Decompressor);

	m_Decompressor->open(m_InputFileStream.get());

	{
		const auto* d = m_Decompressor->getDescriptors();
		memcpy(m_GeomCacheDescs, d, getAttributeCount(d) * sizeof(m_GeomCacheDescs[0]));
	}

	m_DescIndex_points   = getAttributeIndex(m_GeomCacheDescs, nvcSEMANTIC_POINTS  );
	m_DescIndex_normals  = getAttributeIndex(m_GeomCacheDescs, nvcSEMANTIC_NORMALS );
	m_DescIndex_tangents = getAttributeIndex(m_GeomCacheDescs, nvcSEMANTIC_TANGENTS);
	m_DescIndex_uv0      = getAttributeIndex(m_GeomCacheDescs, nvcSEMANTIC_UV0     );
	m_DescIndex_colors   = getAttributeIndex(m_GeomCacheDescs, nvcSEMANTIC_COLORS  );

//	printf("m_DescIndex_points               =%d\n", m_DescIndex_points   );
//	printf("m_DescIndex_normals              =%d\n", m_DescIndex_normals  );
//	printf("m_DescIndex_tangents             =%d\n", m_DescIndex_tangents );
//	printf("m_DescIndex_uv0                  =%d\n", m_DescIndex_uv0      );
//	printf("m_DescIndex_colors               =%d\n", m_DescIndex_colors   );

	prefetch(0, 1);
	return good();
}

bool GeomCache::close() {
	m_Decompressor.release();
	m_InputFileStream.release();
	m_DescIndex_points   = -1;
	m_DescIndex_normals  = -1;
	m_DescIndex_tangents = -1;
	m_DescIndex_uv0      = -1;
	m_DescIndex_colors   = -1;

	return true;
}

bool GeomCache::good() const {
	return static_cast<bool>(m_Decompressor)
	    && static_cast<bool>(m_InputFileStream)
	;
}

void GeomCache::prefetch(size_t currentFrame, size_t range) {
	m_Decompressor->prefetch(currentFrame, range);
}

// Playback.
void GeomCache::setCurrentFrame(float currentTime) {
	m_CurrentTime = currentTime;
	m_CurrentFrame = getFrameIndexByTime(currentTime);
}

void GeomCache::setCurrentFrameIndex(size_t currentFrameIndex) {
	m_CurrentFrame = currentFrameIndex;
	m_CurrentTime = getTimeByFrameIndex(currentFrameIndex);
}

// + function to get geometry data to render.
bool GeomCache::assignCurrentDataToMesh(OutputGeomCache& outputGecomCache) {
	if(! good()) {
		return false;
	}

	GeomCacheData geomCacheData {};

	const size_t frameIndex = getFrameIndexByTime(m_CurrentTime);
	if(frameIndex == ~0u) {
		return false;
	}
	prefetch(frameIndex, 1);

	if(! m_Decompressor->getData(m_CurrentTime, geomCacheData)) {
		return false;
	}
    if (geomCacheData.meshCount == 0 || geomCacheData.submeshCount == 0) {
        return false;
    }

	if(geomCacheData.vertices == nullptr) {
		freeGeomCacheData(geomCacheData, geomCacheData.vertexCount);
		return false;
	}

    outputGecomCache.meshes.assign(geomCacheData.meshes, geomCacheData.meshes + geomCacheData.meshCount);
    outputGecomCache.submeshes.assign(geomCacheData.submeshes, geomCacheData.submeshes + geomCacheData.submeshCount);

	// indices
	{
		outputGecomCache.indices.resize(geomCacheData.indexCount);
		const auto* p = static_cast<const int32_t*>(geomCacheData.indices);
		if(p) {
			memcpy(outputGecomCache.indices.data(), p, outputGecomCache.indices.size() * sizeof(outputGecomCache.indices[0]));
		}
	}

	// points
//	printf("m_DescIndex_points=%d, geomCacheData.vertexCount=%zd\n", m_DescIndex_points, geomCacheData.vertexCount);
	if(m_DescIndex_points >= 0) {
		outputGecomCache.points.resize(geomCacheData.vertexCount);
		const auto* p = geomCacheData.vertices[m_DescIndex_points];
		convertDataArrayToFloat3(
			  outputGecomCache.points.data()
			, p
			, outputGecomCache.points.size()
			, m_GeomCacheDescs[m_DescIndex_points].format
		);
	}

	// normals
	if(m_DescIndex_normals >= 0) {
		outputGecomCache.normals.resize(geomCacheData.vertexCount);
		const auto* p = geomCacheData.vertices[m_DescIndex_normals];
		convertDataArrayToFloat3(
			  outputGecomCache.normals.data()
			, p
			, outputGecomCache.normals.size()
			, m_GeomCacheDescs[m_DescIndex_normals].format
		);
	}

	// tangents
	if(m_DescIndex_tangents >= 0) {
		outputGecomCache.tangents.resize(geomCacheData.vertexCount);
		const auto* p = geomCacheData.vertices[m_DescIndex_tangents];
		convertDataArrayToFloat4(
			  outputGecomCache.tangents.data()
			, p
			, outputGecomCache.tangents.size()
			, m_GeomCacheDescs[m_DescIndex_tangents].format
		);
	}

	// uv0
	if(m_DescIndex_uv0 >= 0) {
		outputGecomCache.uv0.resize(geomCacheData.vertexCount);
		const auto* p = geomCacheData.vertices[m_DescIndex_uv0];
		convertDataArrayToFloat2(
			  outputGecomCache.uv0.data()
			, p
			, outputGecomCache.uv0.size()
			, m_GeomCacheDescs[m_DescIndex_uv0].format
		);
	}

	// colors
	if(m_DescIndex_colors >= 0) {
		outputGecomCache.colors.resize(geomCacheData.vertexCount);
		const auto* p = geomCacheData.vertices[m_DescIndex_colors];
		convertDataArrayToFloat4(
			  outputGecomCache.colors.data()
			, p
			, outputGecomCache.colors.size()
			, m_GeomCacheDescs[m_DescIndex_colors].format
		);
	}

//	freeGeomCacheData(geomCacheData, m_AttributeCount);
	return true;
}

} // namespace nvc
