// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/aligned_alloc.h"
#include "pomdog/memory/alignment.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdlib>
#include <new>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {

template <typename T>
class AlignedNew {
public:
    static void* operator new(size_t size)
    {
        constexpr size_t alignment = alignof(T);
        static_assert(alignment > 8);
        static_assert(isPowerOfTwo(alignment), "Must be integer power of 2.");

        const auto ptr = pomdog::memory::aligned_alloc(alignment, size);
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        return ptr;
    }

    static void operator delete(void* ptr)
    {
        pomdog::memory::aligned_free(ptr);
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

} // namespace pomdog::memory
