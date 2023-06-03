// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct POMDOG_EXPORT GamepadUUID final {
    std::uint16_t busType = 0;
    std::uint16_t vendorID = 0;
    std::uint16_t productID = 0;
    std::uint16_t versionNumber = 0;

    [[nodiscard]] std::string
    toString() const;
};

} // namespace pomdog
