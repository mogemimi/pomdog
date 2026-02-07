// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/aligned_alloc.h"
#include "pomdog/memory/alignment.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(__APPLE_CC__)
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {
namespace {

#define POMDOG_ALIGNED_ALLOC_MODE_WINDOWS 0
#define POMDOG_ALIGNED_ALLOC_MODE_STD_CPP 1
#define POMDOG_ALIGNED_ALLOC_MODE_POSIX 2

#if defined(_MSC_VER)
#define POMDOG_ALIGNED_ALLOC_MODE POMDOG_ALIGNED_ALLOC_MODE_WINDOWS
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201112L)
#define POMDOG_ALIGNED_ALLOC_MODE POMDOG_ALIGNED_ALLOC_MODE_STD_CPP
#elif defined(__APPLE_CC__) && (__APPLE_CC__ >= 6000) && defined(TARGET_OS_MAC) && TARGET_OS_MAC && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_15)
#define POMDOG_ALIGNED_ALLOC_MODE POMDOG_ALIGNED_ALLOC_MODE_STD_CPP
#else
#define POMDOG_ALIGNED_ALLOC_MODE POMDOG_ALIGNED_ALLOC_MODE_POSIX
#endif

} // namespace

[[nodiscard]] void*
aligned_alloc(size_t alignment, size_t size)
{
    if (alignment < sizeof(void*)) [[unlikely]] {
        return nullptr;
    }
    if (!memory::isPowerOfTwo(alignment)) [[unlikely]] {
        return nullptr;
    }

#if (POMDOG_ALIGNED_ALLOC_MODE == POMDOG_ALIGNED_ALLOC_MODE_WINDOWS)
    auto ptr = ::_aligned_malloc(size, alignment);

#elif (POMDOG_ALIGNED_ALLOC_MODE == POMDOG_ALIGNED_ALLOC_MODE_STD_CPP)
    auto ptr = std::aligned_alloc(alignment, size);

#elif (POMDOG_ALIGNED_ALLOC_MODE == POMDOG_ALIGNED_ALLOC_MODE_POSIX)
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        ptr = nullptr;
    }

#else
#error "Sorry, this platform is not supported."
#endif
    return ptr;
}

void aligned_free(void* ptr)
{
    if (ptr == nullptr) [[unlikely]] {
        // NOTE: Passing nullptr is safe and has no effect.
        return;
    }

#if (POMDOG_ALIGNED_ALLOC_MODE == POMDOG_ALIGNED_ALLOC_MODE_WINDOWS)
    ::_aligned_free(ptr);

#elif (POMDOG_ALIGNED_ALLOC_MODE == POMDOG_ALIGNED_ALLOC_MODE_STD_CPP) || \
    (POMDOG_ALIGNED_ALLOC_MODE == POMDOG_ALIGNED_ALLOC_MODE_POSIX)
    // NOTE: When using `std::aligned_alloc` or `posix_memalign`, you can call `free(3)`.
    free(ptr);

#else
#error "Sorry, this platform is not supported."
#endif
}

} // namespace pomdog::memory
