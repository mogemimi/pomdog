// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <string>

namespace Pomdog {

struct POMDOG_EXPORT GamepadUUID final {
    std::uint16_t BusType = 0;
    std::uint16_t VendorID = 0;
    std::uint16_t ProductID = 0;
    std::uint16_t VersionNumber = 0;

    std::string ToString() const;
};

} // namespace Pomdog
