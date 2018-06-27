//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

void RunTest_Types();
void RunTest_MemoryStream();
void RunTest_FileStream();
void RunTest_Alembic();


int main(int argc, char *argv[])
{
    std::map<std::string, std::function<void()>> tests{
        { "Types", RunTest_Types },
        { "MemoryStream", RunTest_MemoryStream },
        { "FileStream", RunTest_FileStream },
        { "Alembic", RunTest_Alembic },

        // add new test here
    };

    if (argc > 1) {
        for (int ai = 1; ai < argc; ++ai) {
            auto it = tests.find(argv[ai]);
            if (it !=tests.end()) {
                it->second();
            }
            else {

            }
        }
    }
    else {
        for (auto& kvp : tests)
            kvp.second();
    }

    return 0;
}
