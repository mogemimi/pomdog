// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/alignment.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(__APPLE_CC__)
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#endif
#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <cstdlib>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

template <typename T>
class AlignedNew {
public:
    static void* operator new(size_t size)
    {
        constexpr size_t alignment = __alignof(T);
        static_assert(alignment > 8, "");
        static_assert(IsPowerOfTwo(alignment), "Must be integer power of 2.");

#if defined(_MSC_VER)
        auto ptr = ::_aligned_malloc(size, alignment);
#elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201112L)) || \
    (defined(__APPLE_CC__) && (__APPLE_CC__ >= 6000) && defined(TARGET_OS_MAC) && TARGET_OS_MAC && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_15))
        auto ptr = std::aligned_alloc(alignment, size);
#else
        void* ptr = nullptr;
        if (posix_memalign(&ptr, alignment, size) != 0) {
            ptr = nullptr;
        }
#endif

        if (nullptr == ptr) {
            throw std::bad_alloc();
        }
        return ptr;
    }

    static void operator delete(void* ptr)
    {
#if defined(_MSC_VER)
        ::_aligned_free(ptr);
#else
        // NOTE: When using `std::aligned_alloc` or `posix_memalign`, you can call `free(3)`.
        free(ptr);
#endif
    }

    static void* operator new[](size_t size)
    {
        return operator new(size);
    }

    static void operator delete[](void* ptr)
    {
        operator delete(ptr);
    }
};

} // namespace pomdog::detail
