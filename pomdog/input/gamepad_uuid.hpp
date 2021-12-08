// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct POMDOG_EXPORT GamepadUUID final {
    std::uint16_t BusType = 0;
    std::uint16_t VendorID = 0;
    std::uint16_t ProductID = 0;
    std::uint16_t VersionNumber = 0;

    std::string ToString() const;
};

} // namespace pomdog
