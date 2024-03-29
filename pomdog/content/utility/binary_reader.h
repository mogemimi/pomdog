// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
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
read(const std::uint8_t* data)
{
    POMDOG_ASSERT(data);
    static_assert(std::is_trivially_copyable_v<T>, "You can only use POD types.");
    static_assert(std::is_standard_layout_v<T>, "You can only use POD types.");
    T value;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

} // namespace pomdog::detail::BinaryReader
