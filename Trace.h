//Overview of a C++ TRACE() implementation
//This header must be included before all others in each source file.
 
#pragma once
 
#if ! defined NDEBUG
    #define DEBUG_MODE_ENABLED 1
#else
    #define DEBUG_MODE_ENABLED 0
#endif
 
enum {debug_mode_enabled= DEBUG_MODE_ENABLED};
 
#include <cstdio>
 
//Variadic Macros, officially since C++11 (and C99 for info), but maybe before as compiler extension
#define TRACE(...)    do {if (debug_mode_enabled) std::fprintf(stderr, __VA_ARGS__);} while(0)
 
//Must be used like printf(), for example:
//TRACE("Hello, %s!", "trace");
