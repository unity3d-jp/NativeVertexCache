//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "GeomCache.h"
#include "Plugin/Compression/NullDecompressor.h"

namespace nvc {

GeomCache::GeomCache()
{
	close();
}

GeomCache::~GeomCache()
{
}

bool GeomCache::open(const char* nvcFilename) {
	m_InputFileStream = std::unique_ptr<FileStream>(
		new FileStream(
			  nvcFilename
			, FileStream::OpenModes::Random_ReadOnly
		)
	);
	assert(m_InputFileStream && m_InputFileStream->canRead());

	m_Decompressor = std::unique_ptr<IDecompressor>(
		new NullDecompressor()
	);
	assert(m_Decompressor);

	preload(0.0f, 1.0f);
	return good();
}

bool GeomCache::close() {
	m_Decompressor.release();
	m_InputFileStream.release();
	return true;
}

bool GeomCache::good() const {
	return static_cast<bool>(m_Decompressor)
	    && static_cast<bool>(m_InputFileStream)
	;
}

void GeomCache::preload(float currentTime, float range) {
	///! @todo : implement
	currentTime, range;
}

// Playback.
void GeomCache::setCurrentFrame(float currentTime) {
	m_CurrentTime = currentTime;
}

// + function to get geometry data to render.
bool GeomCache::assignCurrentDataToMesh(OutputGeomCache& outputGecomCache) {
	if(! good()) {
		return false;
	}

	GeomCacheData geomCacheData {};
	if(! m_Decompressor->getData(m_CurrentTime, geomCacheData)) {
		return false;
	}

//	class OutputGeomCache
//	{
//	public:
//	    std::vector<int> indices;
//	    std::vector<float3> points;
//	    std::vector<float3> normals;
//	    std::vector<float4> tangents;
//	    std::vector<float2> uvs;
//	    std::vector<float4> colors;
//	};
///! @TODO:	@@@ copy geomCacheData to outputGecomCache @@@

	// indices
	outputGecomCache.indices.resize(geomCacheData.indexCount);

	// points
	{
		
	}

	// normals
	// tangents
	// uvs
	// colors

	freeGeomCacheData(geomCacheData, geomCacheData.vertexCount);
	return true;
}

} // namespace nvc
