// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <span>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Fills the memory of a span with a repeated byte value using std::memset.
///
/// This is a low-level memory operation that writes the same byte repeatedly into the
/// memory backing the span. It is safe only for trivially copyable types where such
/// a byte fill is semantically valid (e.g., uint8_t, int, float for zero-fill, etc.).
/// For semantic safety, only use this on types where filling with a uniform bytevalue
/// is meaningful.
template <typename T>
    requires(sizeof(T) == 1 && std::is_trivially_copyable_v<T>)
void memsetSpan(std::span<T> span, u8 value)
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    std::memset(span.data(), static_cast<unsigned char>(value), span.size_bytes());
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

} // namespace pomdog
