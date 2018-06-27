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

	AutoPrepareCleanFile apcfSrc { src_filename };
	AutoPrepareCleanFile apcfDst { dst_filename };

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
	const char* filename = "fs_test_benchmark.file";
	AutoPrepareCleanFile apcfFilename { filename };

	for(int iBufSize = 1; iBufSize <= 256; iBufSize *= 2) {
		ByteArray buf(1024*64 * iBufSize);
		FillRandom(buf, 0x1234, 0x4321);
		size_t nLoop = 1024*1024*1024 / buf.size();

		const auto t0 = GetHighResolutionClock();
		{
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
		printf("TestFileStream: Sequential write speed is %f MB/sec (bufsize=%zdKB)\n", totalBytes / dt / 1024/1024, buf.size()/1024);
	}
}

static void test2() {
	const char* filename = "fs_test_randomaccess.file";

	const size_t totalSize	= 10 * 1024ull * 1024ull * 1024ull;
	const size_t nBlock		= 65536;
	const size_t blockSize	= totalSize / nBlock;

	AutoPrepareCleanFile apcfFilename { filename, false };
	// random access (write)
	{
		FileStream fs(
			  filename
			, FileStream::OpenModes::Random_ReadWrite
		);
		fs.setLength(totalSize);

		std::vector<int64_t> blockOffsets(nBlock);
		for(size_t i = 0; i < blockOffsets.size(); ++i) {
			blockOffsets[i] = i * blockSize;
		}
		std::random_shuffle(blockOffsets.begin(), blockOffsets.end());

		std::vector<uint64_t> buf(blockSize / sizeof(uint64_t));

		const auto t0 = GetHighResolutionClock();
		auto lastStopWatch = t0;
		int64_t tCnt = 0;
		for(const int64_t offset : blockOffsets) {
			{
				++tCnt;
				auto t = GetHighResolutionClock();
				if(GetSeconds(lastStopWatch, t) > 1.0) {
					lastStopWatch = t;
					printf("%zd/%zd (%4.1f%%)\r"
						   , tCnt
						   , blockOffsets.size()
						   , 100.0 * tCnt / blockOffsets.size()
					);
				}
			}
			fs.seek(offset, Stream::SeekOrigin::Begin);
			FillRandom(buf, offset, 1);
			fs.write(buf.data(), buf.size() * sizeof(buf[0]));
		}
		const auto t1 = GetHighResolutionClock();
		printf("%s: %6.2f seconds to write %zd total bytes. (block size = %zd bytes)\n"
			   , __FUNCTION__
			   , GetSeconds(t0, t1)
			   , totalSize
			   , blockSize
		);
	}

	// random access (read/verify)
	{
		assert(IsFileExist(filename));

		FileStream fs(
			  filename
			, FileStream::OpenModes::Random_ReadOnly
		);

		std::vector<int64_t> blockOffsets(nBlock);
		for(size_t i = 0; i < blockOffsets.size(); ++i) {
			blockOffsets[i] = i * blockSize;
		}
		std::random_shuffle(blockOffsets.begin(), blockOffsets.end());

		std::vector<uint64_t> buf(blockSize / sizeof(uint64_t));
		std::vector<uint64_t> refBuf(buf.size());

		const auto t0 = GetHighResolutionClock();
		auto lastStopWatch = t0;
		int64_t tCnt = 0;
		for(const int64_t offset : blockOffsets) {
			{
				++tCnt;
				auto t = GetHighResolutionClock();
				if(GetSeconds(lastStopWatch, t) > 1.0) {
					lastStopWatch = t;
					printf("%zd/%zd (%4.1f%%)\r"
						   , tCnt
						   , blockOffsets.size()
						   , 100.0 * tCnt / blockOffsets.size()
					);
				}
			}
			FillRandom(refBuf, offset, 1);

			fs.seek(offset, Stream::SeekOrigin::Begin);
			fs.read(buf.data(), buf.size() * sizeof(buf[0]));

			assert(0 == memcmp(
				  refBuf.data()
				, buf.data()
				, buf.size() * sizeof(buf[0])
			));
		}
		const auto t1 = GetHighResolutionClock();
		printf("%s: %6.2f seconds to read %zd total bytes. (block size = %zd bytes)\n"
			   , __FUNCTION__
			   , GetSeconds(t0, t1)
			   , totalSize
			   , blockSize
		);
	}
}

void RunTest_FileStream()
{
	test0();
	test1();
	test2();
}
