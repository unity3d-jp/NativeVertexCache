#pragma once

enum FrameType
{
	IFrame,
};

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

	//void assignCurrentDataToMesh(Mesh& mesh);


	//// Sampling.
	//template<typename TDataType>
	//TDataType Sample<TDataType>(float time, const char* semantic);

	//...
	GeomCache(const GeomCache&) = delete;
	GeomCache(GeomCache&&) = delete;
	GeomCache& operator=(const GeomCache&) = delete;
	GeomCache& operator=(GeomCache&&) = delete;
};
