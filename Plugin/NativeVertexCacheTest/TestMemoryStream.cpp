﻿//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"
#include "Plugin/Stream/MemoryStream.h"
#include "Plugin/Pcg.h"

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
	{
		ByteArray ba(1024);
		FillRandom(ba, 0x1234, 0x4321);

		MemoryStream msSrc(
			  ba.size()		// length
			, false			// resizable
		);
		msSrc.write(ba.data(), ba.size());

		MemoryStream msDst(
			  0				// length
			, true			// resizable
		);
		msDst.setLength(msSrc.getLength());
		msDst.seek(0, Stream::SeekOrigin::Begin);
		msSrc.copyTo(&msDst, msSrc.getLength());

		assert(msSrc.getLength() == msDst.getLength());

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
	}
}
