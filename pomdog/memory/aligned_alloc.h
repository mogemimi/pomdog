// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdlib>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {

/// Allocates memory with the specified alignment.
///
/// @param alignment The alignment boundary in bytes (must be a power of two
///        and at least `sizeof(void*)`).
/// @param size The size of the memory block in bytes. Must be an integral
///        multiple of @p alignment.
/// @return A pointer to the allocated memory, or nullptr on failure.
///         Returns nullptr if @p size is not a multiple of @p alignment.
///
/// This function wraps platform-specific aligned allocation mechanisms:
/// - On Windows: uses `_aligned_malloc`.
/// - On POSIX systems: uses `posix_memalign`.
/// - On C++17 and later: uses `std::aligned_alloc`.
///
/// The memory returned must be freed with `aligned_free()`.
[[nodiscard]] void*
aligned_alloc(size_t alignment, size_t size);

/// Frees memory allocated with `aligned_alloc`.
///
/// @param ptr The pointer to memory previously returned by `aligned_alloc`.
///
/// This function wraps the correct deallocation function for each platform:
/// - On Windows: uses `_aligned_free`.
/// - On POSIX and standard C++: uses `free()`.
///
/// Passing nullptr is safe and has no effect.
void aligned_free(void* ptr);

} // namespace pomdog::memory
