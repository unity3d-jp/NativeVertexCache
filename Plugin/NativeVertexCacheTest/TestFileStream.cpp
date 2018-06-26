//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"
#include "Plugin/Stream/FileStream.h"
#include "Plugin/NativeVertexCacheTest/TestUtil.h"

using namespace nvc;
using ByteArray = std::vector<uint8_t>;

static void test0() {
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

static void test1() {
	ByteArray buf(1024*1024);
	FillRandom(buf, 0x1234, 0x4321);
	size_t nLoop = 1024;

	const auto t0 = GetHighResolutionClock();
	{
		const char* filename = "fs_test_benchmark.file";
		FileStream fs(
			  filename
			, FileStream::OpenModes::Random_ReadWrite
		);

		for(size_t iLoop = 0; iLoop < nLoop; ++iLoop) {
			fs.write(buf.data(), buf.size());
		}
	}
	const auto t1 = GetHighResolutionClock();

	const auto dt = GetSeconds(t0, t1);
	const auto totalBytes = (double) (buf.size() * nLoop);
	printf("(%s): Sequential write speed is %f MB/sec\n", __FILE__, totalBytes / dt / 1024/1024);
}

void RunTest_FileStream()
{
	test0();
	test1();
}
