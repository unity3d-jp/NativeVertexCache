#pragma once

#ifdef _WIN32
    #define nvcAPI extern "C" __declspec(dllexport)
#else
    #define nvcAPI extern "C"
#endif
