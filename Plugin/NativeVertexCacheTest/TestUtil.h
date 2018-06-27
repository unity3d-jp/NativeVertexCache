#include "Plugin/Pcg.h"
#include <chrono>

template<class T>
inline bool NearEqual(T a, T b, T eps)
{
    return std::abs(a - b) <= std::abs(eps);
}

inline void ThrowError(const char *format, ...)
{
    char buf[1024];

    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    throw std::runtime_error(buf);
}

template<typename T>
inline void FillRandom(T& vec, uint64_t state = 0x900dbeef, uint64_t inc = 0x87654321) {
    using ValueType = T::value_type;
    Pcg pcg { state, inc };
    for(auto& v : vec) {
        v = static_cast<ValueType>(pcg.getUint32());
    }
}

template<typename T>
inline void FillSequence(T& vec) {
    using ValueType = T::value_type;
    auto x = static_cast<ValueType>(0);
    for(auto& v : vec) {
        v = x;
        ++x;
    }
}

// usage:
//    const auto start_time = GetHighResolutionClock();
//    ... do something ...
//    const auto end_time = GetHighResolutionClock();
//    const double diff_time_in_seconds = GetSeconds(start_time, end_time);
//    printf("%f seconds\n", diff_time_in_seconds);
//
inline std::chrono::high_resolution_clock::time_point GetHighResolutionClock() {
    return std::chrono::high_resolution_clock::now();
}

inline double GetSeconds(std::chrono::high_resolution_clock::time_point begin, std::chrono::high_resolution_clock::time_point end) {
    const auto d = static_cast<std::chrono::duration<double>>(end - begin);
    return d.count();
}

bool RemoveFile(const char* filename);
bool IsFileExist(const char* filename);
bool IsFileReadable(const char* filename);
bool IsFileWritable(const char* filename);

class AutoPrepareCleanFile {
public:
	AutoPrepareCleanFile(const char* filename, bool removeOnDestructor = true)
		: filename { filename }
		, removeOnDestructor { removeOnDestructor }
	{
		RemoveFile(filename);
	}

	~AutoPrepareCleanFile()
	{
		if(removeOnDestructor) {
			RemoveFile(filename.c_str());
		}
	}

protected:
	std::string filename;
	const bool removeOnDestructor;
};

inline uint64_t hash64(uint64_t x) {
	// The finalizer from MurmerHash3
	// https://stackoverflow.com/a/6867612
	x ^= x >> 33;
	x *= 0xff51afd7ed558ccd;
	x ^= x >> 33;
	x *= 0xc4ceb9fe1a85ec53;
	x ^= x >> 33;
	return x;
}

inline int64_t hash64(int64_t x) {
	return static_cast<int64_t>(hash64(static_cast<uint64_t>(x)));
}
