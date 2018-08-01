// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Input/GamepadUUID.hpp"
#include "../Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>

namespace Pomdog {
namespace {

template <typename T>
T SwapEndian(T u)
{
#if defined(POMDOG_BYTEORDER_BIG_ENDIAN)
    return u;
#else
    static_assert(sizeof(std::uint8_t) == 1, "");
    union {
        T u;
        std::uint8_t u8[sizeof(T)];
    } source, dest;
    source.u = u;
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        dest.u8[i] = source.u8[(sizeof(T) - i) - 1];
    }
    return dest.u;
#endif
}

} // namespace

std::string GamepadUUID::ToString() const
{
    std::array<std::uint16_t, 8> uuid;
    std::fill(std::begin(uuid), std::end(uuid), static_cast<std::uint16_t>(0));
    if ((VendorID != 0) && (ProductID != 0)) {
        uuid[0] = BusType;
        uuid[1] = 0;
        uuid[2] = VendorID;
        uuid[3] = 0;
        uuid[4] = ProductID;
        uuid[5] = 0;
        uuid[6] = VersionNumber;
        uuid[7] = 0;
    }

    std::string s;
    for (auto u : uuid) {
        s += StringHelper::Format("%04x", SwapEndian(u));
    }
    return s;
}

} // namespace Pomdog
