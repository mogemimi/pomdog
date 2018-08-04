// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace Pomdog {
namespace Detail {

constexpr bool IsPowerOfTwo(int x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

template <typename T>
class AlignedNew {
public:
    static void* operator new(size_t size)
    {
        constexpr size_t alignment = __alignof(T);
        static_assert(alignment > 8, "");
        static_assert(IsPowerOfTwo(static_cast<int>(alignment)), "Must be integer power of 2.");

#if defined(_MSC_VER)
        auto ptr = ::_aligned_malloc(size, alignment);
#elif (__STDC_VERSION__ > 201112L)
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

} // namespace Detail
} // namespace Pomdog
