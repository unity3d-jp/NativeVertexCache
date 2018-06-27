#pragma once

#include "../Types.h"
#include "../nvcAPI.h"

#ifdef _WIN32
    #ifdef NVCABC_IMPL
        #define nvcabcAPI extern "C" __declspec(dllexport)
    #else
        #define nvcabcAPI extern "C" __declspec(dllimport)
    #endif
#else
    #define nvcabcAPI extern "C"
#endif


namespace nvcabc {

struct AlembicImportOptions
{
    bool multithreading = true;
};

struct AlembicGeometries
{
    // GeomPtrs use nvcIGCRelease as deleter
    using GeomPtr = std::shared_ptr<nvc::InputGeomCache>;

    GeomPtr geometry;
    std::vector<float> timesamples;
};

nvcabcAPI bool AlembicToGeomCache(const char *path_to_abc, const AlembicImportOptions& options, AlembicGeometries& result);

} // namespace nvcabc
