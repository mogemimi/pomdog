// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <span>
#include <stdexcept>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4710)

/// Copies elements from a source span to a destination span using memcpy.
///
/// This function performs a raw memory copy of the contents of the source span
/// into the destination span. It ensures at compile time that the element type
/// is trivially copyable. It also checks at runtime that the source span does
/// not exceed the size of the destination span.
///
/// If src and dst refer to overlapping memory, the behavior is undefined.
/// For overlapping ranges, use memmove instead of memcpy.
template <typename To, typename From, std::size_t ToExtent = std::dynamic_extent, std::size_t FromExtent = std::dynamic_extent>
    requires(std::is_trivially_copyable_v<To> && std::is_same_v<To, std::remove_const_t<From>>)
void memcpySpan(std::span<To, ToExtent> dst, std::span<From, FromExtent> src)
{
    static_assert(std::is_trivially_copyable_v<From>);

    if constexpr ((ToExtent != std::dynamic_extent) && (FromExtent != std::dynamic_extent)) {
        static_assert(FromExtent <= ToExtent, "memcpySpan: source span is larger than destination span.");
    }

    if (src.size() > dst.size()) {
        throw std::out_of_range("memcpySpan: source span is larger than destination span.");
    }

    POMDOG_CLANG_SUPPRESS_WARNING_PUSH
#if defined(__clang__)
#if __has_warning("-Wunsafe-buffer-usage-in-libc-call")
    POMDOG_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage-in-libc-call")
#endif
#endif
    std::memcpy(dst.data(), src.data(), src.size_bytes());
    POMDOG_CLANG_SUPPRESS_WARNING_POP
}

POMDOG_MSVC_SUPPRESS_WARNING_POP

} // namespace pomdog
