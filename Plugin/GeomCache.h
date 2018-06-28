#pragma once

#include "Plugin/OutputGeomCache.h"
#include "Plugin/GeomCacheData.h"
#include "Plugin/Compression/IDecompressor.h"
#include "Plugin/Compression/NulLDecompressor.h"
#include "Plugin/Stream/FileStream.h"

namespace nvc {

class GeomCache final
{

public:
	GeomCache();
	~GeomCache();

	bool open(const char* nvcFilename);
	bool close();
	bool good() const;

	// Caching
	//  I                                I
	// [X][o][o][o][o][o][o][][][][][][][X][o][o][o][o]][o][o][][][][][][]
	// |-------------------|  skip =>   |-------------------|

	//  I           I                          I
	// [X][o][o][o][o][o][o][][][][][][][][][][X][o][o][o][o]][o][o][][][][][][]
	//              |-------------------|  <= skip
	void prefetch(size_t currentTime, size_t range);

	// Playback.
	void setCurrentFrame(float currentTime);
	void setCurrentFrameIndex(size_t currentFrameIndex);
	// + function to get geometry data to render.

	bool assignCurrentDataToMesh(OutputGeomCache& mesh);

	size_t getFrameCount() const {
		return m_Decompressor->getFrameCount();
	}

	size_t getFrameIndexByTime(float time) const {
		return m_Decompressor->getFrameIndex(time);
	}

	float getTimeByFrameIndex(size_t frameIndex) const {
		return m_Decompressor->getFrameTime(frameIndex);
	}

	//// Sampling.
	//template<typename TDataType>
	//TDataType Sample<TDataType>(float time, const char* semantic);

	//...
	GeomCache(const GeomCache&) = delete;
	GeomCache(GeomCache&&) = delete;
	GeomCache& operator=(const GeomCache&) = delete;
	GeomCache& operator=(GeomCache&&) = delete;

protected:
	std::unique_ptr<IDecompressor> m_Decompressor {};
//	std::unique_ptr<NullDecompressor> m_Decompressor {};
	std::unique_ptr<FileStream> m_InputFileStream {};
//	const GeomCacheDesc* m_GeomCacheDescs {};
//	size_t m_GeomCacheDescsNum = 0;
	GeomCacheDesc m_GeomCacheDescs[GEOM_CACHE_MAX_DESCRIPTOR_COUNT + 1] {};

	size_t m_AttributeCount = 0;
	int m_DescIndex_points = -1;
	int m_DescIndex_normals = -1;
	int m_DescIndex_tangents = -1;
	int m_DescIndex_uv0 = -1;
	int m_DescIndex_colors = -1;

	float m_CurrentTime = 0.0f;
	size_t m_CurrentFrame = 0;
};

} // namespace nvc
