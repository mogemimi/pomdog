// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct POMDOG_EXPORT GamepadUUID final {
    u16 busType = 0;
    u16 vendorID = 0;
    u16 productID = 0;
    u16 versionNumber = 0;

    [[nodiscard]] std::string
    toString() const;
};

} // namespace pomdog
