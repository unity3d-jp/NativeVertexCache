//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"
#include "Plugin/Stream/FileStream.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"

using namespace nvc;
using ByteArray = std::vector<uint8_t>;

void RunTest_FileStream()
{
	ByteArray ba(1024);
//	FillRandom(ba, 0x1234, 0x4321);
	FillSequence(ba);

	const char* src_filename = "test_src.file";
	const char* dst_filename = "test_dst.file";

	FileStream msSrc(
		  src_filename
		, FileStream::OpenModes::Random_ReadWrite
	);
	msSrc.write(ba.data(), ba.size());
//msSrc.flush();

	FileStream msDst(
		  dst_filename
		, FileStream::OpenModes::Random_ReadWrite
	);
	msDst.setLength(msSrc.getLength());
	msDst.seek(0, Stream::SeekOrigin::Begin);

	msSrc.seek(0, Stream::SeekOrigin::Begin);
	msSrc.copyTo(&msDst, msSrc.getLength());
//msDst.flush();

	{
		const auto srcLen = msSrc.getLength();
		const auto dstLen = msDst.getLength();
		assert(srcLen == dstLen);
	}

	ByteArray bb(ba.size());
	msDst.seek(0, Stream::SeekOrigin::Begin);
	msDst.read(bb.data(), bb.size());

	for(size_t i = 0; i < ba.size(); ++i) {
		if(ba[i] != bb[i]) {
			printf("%s(%d) : ba[%zd](0x%02x) != bb[%zd](0x%02x)\n"
				   , __FUNCTION__, __LINE__
				   , i, ba[i], i, bb[i]);
		}
	}
	assert(0 == memcmp(
		  ba.data()
		, bb.data()
		, bb.size()
	));

	assert(msSrc.getLength() == ba.size());
	assert(msDst.getLength() == bb.size());
}
