#pragma once

#include "Plugin/OutputGeomCache.h"

namespace nvc {

class GeomCache final
{

public:
	GeomCache();
	~GeomCache();

	// Caching
	//  I                                I
	// [X][o][o][o][o][o][o][][][][][][][X][o][o][o][o]][o][o][][][][][][]
	// |-------------------|  skip =>   |-------------------|

	//  I           I                          I
	// [X][o][o][o][o][o][o][][][][][][][][][][X][o][o][o][o]][o][o][][][][][][]
	//              |-------------------|  <= skip
	void preload(float currentTime, float range);

	// Playback.
	void setCurrentFrame(float currentTime);
	// + function to get geometry data to render.

	void assignCurrentDataToMesh(OutputGeomCache& mesh);


	//// Sampling.
	//template<typename TDataType>
	//TDataType Sample<TDataType>(float time, const char* semantic);

	//...
	GeomCache(const GeomCache&) = delete;
	GeomCache(GeomCache&&) = delete;
	GeomCache& operator=(const GeomCache&) = delete;
	GeomCache& operator=(GeomCache&&) = delete;
};

} // namespace nvc
