//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"
#include "./AbcToNvc.h"

void RunTest_Types();
void RunTest_MemoryStream();
void RunTest_FileStream();
void RunTest_Alembic();
void RunTest_AlembicToNvc();


int main(int argc, char *argv[])
{
	using namespace nvc;

	std::map<std::string, std::function<void()>> tests{
        { "Types", RunTest_Types },
        { "MemoryStream", RunTest_MemoryStream },
        { "+FileStream", RunTest_FileStream },
        { "Alembic", RunTest_Alembic },
        { "+AlembicToNvc", RunTest_AlembicToNvc },

        // If first char of the argument name is '+', it means "opt-in" option.
        // add new test here
    };

	AbcToNvcCompressionMethod compressionMethod = AbcToNvcCompressionMethod::Null;

	if (argc > 1) {
        for (int ai = 1; ai < argc; ++ai) {
            if(_stricmp(argv[ai], "--cmp-null") == 0) {
				compressionMethod = AbcToNvcCompressionMethod::Null;
				continue;
            }
            if(_stricmp(argv[ai], "--cmp-quantisation") == 0) {
				compressionMethod = AbcToNvcCompressionMethod::Quantisation;
				continue;
            }
            if(_stricmp(argv[ai], "--abc-to-nvc") == 0) {
                return AbcToNvc(argv[ai+1], argv[ai+2], compressionMethod);
            }
            auto it = tests.find(argv[ai]);
            if (it !=tests.end()) {
                it->second();
            }
            else {

            }
        }
    }
    else {
        for (auto& kvp : tests) {
            if(kvp.first[0] != '+') {
                kvp.second();
            }
        }
    }

    return 0;
}
