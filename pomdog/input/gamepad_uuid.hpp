// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct POMDOG_EXPORT GamepadUUID final {
    std::uint16_t BusType = 0;
    std::uint16_t VendorID = 0;
    std::uint16_t ProductID = 0;
    std::uint16_t VersionNumber = 0;

    std::string ToString() const;
};

} // namespace Pomdog
