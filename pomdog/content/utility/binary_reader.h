// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/memcpy_span.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/bit_cast_span.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::BinaryReader {

template <typename T, class Stream>
[[nodiscard]] std::vector<T>
readArray(Stream& stream, std::size_t elementCount)
{
    static_assert(std::is_trivially_copyable_v<T>, "You can only use POD types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use POD types.");

    POMDOG_ASSERT(stream);
    POMDOG_ASSERT(elementCount > 0);

    std::vector<T> result;
    result.reserve(elementCount + 1);
    result.resize(elementCount);
    stream.read(reinterpret_cast<char*>(result.data()), static_cast<int>(sizeof(T) * result.size()));
    return result;
}

template <typename T, std::size_t ElementCount, class Stream>
[[nodiscard]] std::array<T, ElementCount>
readArray(Stream& stream)
{
    static_assert(std::is_trivially_copyable_v<T>, "You can only use POD types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use POD types.");
    POMDOG_ASSERT(stream);

    std::array<T, ElementCount> result;
    stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
    return result;
}

template <typename T, class Stream>
[[nodiscard]] T
read(Stream& stream)
{
    POMDOG_ASSERT(stream);
    static_assert(std::is_trivially_copyable_v<T>, "You can only use POD types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use POD types.");
    return std::move(readArray<T, 1>(stream)[0]);
}

template <typename T>
[[nodiscard]] bool
canRead(std::size_t byteLength)
{
    return sizeof(T) <= byteLength;
}

template <typename T>
[[nodiscard]] T
read(const u8* data)
{
    POMDOG_ASSERT(data);
    static_assert(std::is_trivially_copyable_v<T>, "You can only use POD types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use POD types.");
    T value;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

template <class T>
[[nodiscard]] auto read(std::span<const u8> bin) noexcept -> T
{
    static_assert(std::is_fundamental_v<T>);
    static_assert(std::is_arithmetic_v<T>);
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_pointer_v<T>);

    constexpr std::size_t requiredSize = sizeof(T);
    POMDOG_ASSERT(bin.size() >= requiredSize);
    T result = T(0);

    const auto dstSpan = toByteArraySpanFromSingle<u8>(result);
    const auto srcSpan = std::span<const u8>{bin.data(), requiredSize};

    memcpySpan(dstSpan, srcSpan);
    return result;
}

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
template <class T>
[[nodiscard]] std::tuple<T, std::span<const u8>, std::unique_ptr<Error>>
readAs(std::span<const u8> bin) noexcept
{
    static_assert(std::is_fundamental_v<T>);
    static_assert(std::is_arithmetic_v<T>);
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_pointer_v<T>);

    constexpr std::size_t requiredSize = sizeof(T);

    if (bin.size() < requiredSize) {
        return std::make_tuple(T{}, bin, errors::make("bin.size() must be >= requiredSize"));
    }
    const auto result = read<T>(bin);
    return std::make_tuple(result, bin.subspan(requiredSize), nullptr);
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

template <class T>
[[nodiscard]] std::unique_ptr<Error>
readAs(std::span<const u8>& bin, T& result) noexcept
{
    static_assert(std::is_fundamental_v<T>);
    static_assert(std::is_arithmetic_v<T>);
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_pointer_v<T>);

    constexpr std::size_t requiredSize = sizeof(T);

    if (bin.size() < requiredSize) {
        return errors::make("bin.size() must be >= requiredSize");
    }
    result = read<T>(bin);
    bin = bin.subspan(requiredSize);
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
seek(std::span<const u8>& bin, std::size_t offset) noexcept;

} // namespace pomdog::detail::BinaryReader
