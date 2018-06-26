#include "Plugin/PrecompiledHeader.h"
#include "./TestUtil.h"

#if defined(_MSC_VER)
#include <io.h> // _unlink(), _access()
#endif

bool RemoveFile(const char* filename) {
	return _unlink(filename) == 0;
}

bool IsFileExist(const char* filename) {
	return _access(filename, 0) == 0;
}

bool IsFileReadable(const char* filename) {
	return _access(filename, 4) == 0 || _access(filename, 6) == 0;
}

bool IsFileWritable(const char* filename) {
	return _access(filename, 2) == 0 || _access(filename, 6) == 0;
}
