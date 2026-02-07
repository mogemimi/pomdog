// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <span>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Converts a span of trivially copyable objects to a byte-level view.
///
/// This function provides a reinterpretation of a given `std::span` of
/// trivially copyable elements as a `std::span` of bytes (`u8` or `i8`).
/// It is useful in low-level programming contexts such as graphics or serialization,
/// where raw memory views are needed (e.g., uploading vertex buffers to the GPU).
template <typename To, typename From, std::size_t Extent = std::dynamic_extent>
[[nodiscard]] std::span<To>
toByteArraySpan(std::span<From, Extent> from) noexcept
{
    static_assert(std::is_fundamental_v<To>);
    static_assert(std::is_integral_v<To>);
    static_assert(std::is_arithmetic_v<To>);
    static_assert(sizeof(To) == 1);
    static_assert(!std::is_pointer_v<To>);

    static_assert(std::is_trivially_copyable_v<From>, "From must be trivially copyable");
    static_assert(std::is_standard_layout_v<From>);

    // NOTE:
    // This function uses `reinterpret_cast` internally and assumes that
    // `From` is safely reinterpret-castable to a sequence of bytes. It does
    // not perform any runtime checks and is intended for performance-critical use.
    //
    // The returned span is only valid as long as the original `from` data remains valid.
    // Misuse (e.g., using non-trivially copyable types or modifying data through a `const`-view)
    // may result in undefined behavior.
    POMDOG_CLANG_SUPPRESS_WARNING_PUSH
    POMDOG_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage-in-container")
    return std::span<To>{reinterpret_cast<To*>(from.data()), from.size_bytes()};
    POMDOG_CLANG_SUPPRESS_WARNING_POP
}

/// Converts an array of trivially copyable objects to a byte-level view.
template <typename To, typename From, std::size_t Extent>
[[nodiscard]] std::span<To>
toByteArraySpan(std::array<From, Extent>& from) noexcept
{
    return toByteArraySpan<To>(std::span<From, Extent>{from});
}

/// Converts an array of trivially copyable objects to a byte-level view.
template <typename To, typename From, std::size_t Extent>
[[nodiscard]] std::span<To>
toByteArraySpan(const std::array<From, Extent>& from) noexcept
{
    return toByteArraySpan<To>(std::span<const From, Extent>{from});
}

/// Converts a trivially copyable object to a byte-level view.
template <typename To, typename From, std::size_t Extent = sizeof(From)>
[[nodiscard]] std::span<To, Extent>
toByteArraySpanFromSingle(From& from) noexcept
{
    static_assert(std::is_fundamental_v<To>);
    static_assert(std::is_integral_v<To>);
    static_assert(std::is_arithmetic_v<To>);
    static_assert(sizeof(To) == 1);
    static_assert(!std::is_pointer_v<To>);

    static_assert(std::is_trivially_copyable_v<From>, "From must be trivially copyable");
    static_assert(std::is_standard_layout_v<From>);

    return std::span<To, Extent>{reinterpret_cast<To*>(&from), Extent};
}

} // namespace pomdog
