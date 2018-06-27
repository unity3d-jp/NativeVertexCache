#pragma once

#include "GeomCacheData.h"

namespace nvc {

class InputGeomCache final
{
private:
	using FrameDataType = std::pair<float, GeomCacheData>;

	GeomCacheDesc m_Descriptor[GEOM_CACHE_MAX_DESCRIPTOR_COUNT] = {};
	std::vector<FrameDataType> m_Data;

public:
	// last element of desc must be GEOM_CACHE_DESCRIPTOR_END (null terminator)
	InputGeomCache(const GeomCacheDesc *desc);
	~InputGeomCache();

	void addData(float time, const GeomCacheData *data);

	void getDesc(GeomCacheDesc *desc) const;
	// GeomCacheData::data can be nullptr. in that case, only count will be filled.
	void getData(size_t frameIndex, float& frameTime, GeomCacheData* data) const;
	size_t getDataCount() const;

	//...
	InputGeomCache(const InputGeomCache&) = delete;
	InputGeomCache(InputGeomCache&&) = delete;
	InputGeomCache& operator=(const InputGeomCache&) = delete;
	InputGeomCache& operator=(InputGeomCache&&) = delete;
};

} // namespace nvc
