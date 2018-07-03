#pragma once

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

using ImportCallback = void(*)(void *userdata, nvc::InputGeomCache *igc);

struct AlembicImportOptions
{
    bool multithreading = true;
    int callback_interval = 0; // 0: never
    ImportCallback callback = nullptr;
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
