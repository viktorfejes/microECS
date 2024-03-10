#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>

namespace microECS {

// Static assert implementation
#define STATIC_ASSERT(expr, msg) static_assert(expr, msg)

// Runtime assert implementation
#ifdef NDEBUG
#define ASSERT(expr, msg) ((void)0)
#else
void assertFailure(const char* expr, const char* file, int line, const char* msg) {
    std::fprintf(stderr, "Assertion failed: %s, %s, file %s, line %d.\n", expr, msg, file, line);
    std::abort();
}

#define ASSERT(expr, msg)                                                                          \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            assertFailure(#expr, __FILE__, __LINE__, msg);                                         \
        }                                                                                          \
    } while (false)
#endif

} // namespace microECS