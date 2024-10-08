// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/gamepad_uuid.h"
#include "pomdog/basic/compilation_targets.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

template <typename T>
[[nodiscard]] T swapEndian(T u) noexcept
{
    static_assert(std::is_fundamental_v<T>);
    static_assert(std::is_integral_v<T>);

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

std::string GamepadUUID::toString() const
{
    std::array<std::uint16_t, 8> uuid;
    std::fill(std::begin(uuid), std::end(uuid), static_cast<std::uint16_t>(0));
    if ((vendorID != 0) && (productID != 0)) {
        uuid[0] = busType;
        uuid[1] = 0;
        uuid[2] = vendorID;
        uuid[3] = 0;
        uuid[4] = productID;
        uuid[5] = 0;
        uuid[6] = versionNumber;
        uuid[7] = 0;
    }

    std::string s;
    for (auto u : uuid) {
        s += strings::format("%04x", swapEndian(u));
    }
    return s;
}

} // namespace pomdog
