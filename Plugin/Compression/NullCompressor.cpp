//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "NullCompressor.h"


//-----------------------------
// File
//-----------------------------
// Header
//  Descriptor
//    uint32_t itemCount;
//	  char[80] semantic;
//    DataFormat type;
//	  char[80] semantic;
//    DataFormat type;
//    ...
//  uint32_t frameCount;
//  uint32_t windowSize;
//
//-----------------------------
// Frame0: uint32_t offset;
// Frame[1*windowSize]: uint32_t offset;
// ...
// Frame[n*windowSize]: uint32_t offset;
//-----------------------------
// Frames:
//   I-Frame
//     FrameType Type;
//     uint32_t Count;
//     [DataBlob]
//   I-Frame
//     FrameType Type;
//     uint32_t Count;
//     [DataBlob]
// ...

void NullCompressor::Compress(const InputGeomCache& geomCache, Stream* pStream)
{
	// Write header.

	struct Header
	{
		uint32_t FrameCount;
		uint32_t FrameRate;

		uint32_t VertexAttributeCount;
	};
}
