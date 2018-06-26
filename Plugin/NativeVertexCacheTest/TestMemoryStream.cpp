//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"
#include "Plugin/Stream/MemoryStream.h"
#include "Plugin/Pcg.h"

#undef assert
#define assert(expr) \
    if (!(expr)) \
        aFailed(__FILE__, __LINE__)

static void aFailed(const char* filename, size_t line) {
	printf("Assertion failed @ (%s:%zd)\n", filename, line);
	exit(EXIT_FAILURE);
}

using namespace nvc;
using ByteArray = std::vector<uint8_t>;

template<typename T>
static void FillRandom(T& vec, uint64_t state = 0x900dbeef, uint64_t inc = 0x87654321) {
	using ValueType = T::value_type;
	Pcg pcg { state, inc };
	for(auto& v : vec) {
		v = static_cast<ValueType>(pcg.getUint32());
	}
}

void RunTest_MemoryStream()
{
	printf("%s(%d) : begin\n", __FUNCTION__, __LINE__);
	{
printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		ByteArray ba(1024);
		FillRandom(ba, 0x1234, 0x4321);

printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		MemoryStream msSrc(
			  ba.size()		// length
			, false			// resizable
		);
printf("%s(%d) : msSrc.getLength() = %zd\n", __FUNCTION__, __LINE__, msSrc.getLength());

printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		MemoryStream msDst(
			  0				// length
			, true			// resizable
		);

printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
printf("%s(%d) : msSrc.getLength() = %zd\n", __FUNCTION__, __LINE__, msSrc.getLength());
		msDst.setLength(msSrc.getLength());
printf("%s(%d) : msDst.getLength() = %zd\n", __FUNCTION__, __LINE__, msDst.getLength());
		msDst.seek(0, Stream::SeekOrigin::Begin);
		msSrc.copyTo(&msDst, msSrc.getLength());

printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		assert(msSrc.getLength() == msDst.getLength());

printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		ByteArray bb(ba.size());
printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		msDst.seek(0, Stream::SeekOrigin::Begin);
printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		{
			const auto bb_size = bb.size();
printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
			const auto bb_data = bb.data();
printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
			const auto result = msDst.read(bb_data, bb_size);
printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		}
printf("%s(%d) : end\n", __FUNCTION__, __LINE__);

printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
		assert(0 == memcmp(
			  ba.data()
			, bb.data()
			, bb.size()
		));
	}
	printf("%s(%d) : end\n", __FUNCTION__, __LINE__);
}
