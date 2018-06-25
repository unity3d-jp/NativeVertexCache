//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "DataFormat.h"
#include "Types.h"

namespace nvc 
{
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

} // namespace nvc
