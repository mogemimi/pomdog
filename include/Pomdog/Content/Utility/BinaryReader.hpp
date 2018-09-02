// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {

struct POMDOG_EXPORT BinaryReader final {
    template <typename T, class Stream>
    static std::vector<T> ReadArray(Stream& stream, std::size_t elementCount)
    {
        static_assert(std::is_pod<T>::value, "You can only use POD types.");

        POMDOG_ASSERT(stream);
        POMDOG_ASSERT(elementCount > 0);

        std::vector<T> result;
        result.reserve(elementCount + 1);
        result.resize(elementCount);
        stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
        return std::move(result);
    }

    template <typename T, std::size_t ElementCount, class Stream>
    static std::array<T, ElementCount> ReadArray(Stream& stream)
    {
        static_assert(std::is_pod<T>::value, "You can only use POD types.");
        POMDOG_ASSERT(stream);

        std::array<T, ElementCount> result;
        stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
        return std::move(result);
    }

    template <typename T, class Stream>
    static T Read(Stream& stream)
    {
        POMDOG_ASSERT(stream);
        static_assert(std::is_pod<T>::value, "You can only use POD types.");
        return std::move(ReadArray<T, 1>(stream)[0]);
    }

    template <typename T, class Stream>
    static std::vector<T> ReadString(Stream& stream, std::size_t elementCount)
    {
        static_assert(std::is_integral<T>::value, "You can only use POD types.");
        auto result = ReadArray<T>(stream, elementCount);

        // Insert null at the end of a charater array
        result.push_back(0);
        return std::move(result);
    }

    template <typename T>
    static bool CanRead(std::size_t byteLength)
    {
        return sizeof(T) <= byteLength;
    }

    template <typename T>
    static T Read(const std::uint8_t* data)
    {
        POMDOG_ASSERT(data);
        static_assert(std::is_pod<T>::value, "You can only use POD types.");
        T value;
        std::memcpy(&value, data, sizeof(value));
        return std::move(value);
    }
};

} // namespace Detail
} // namespace Pomdog
