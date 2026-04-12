// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// A device identifier for gamepads, composed of bus type, vendor,
/// product, and version fields. Used as a key for controller DB lookups.
struct POMDOG_EXPORT GamepadUUID final {
    /// Bus type (e.g. 0x03 for USB, 0x05 for Bluetooth).
    u16 busType = 0;

    /// USB vendor ID.
    u16 vendorID = 0;

    /// USB product ID.
    u16 productID = 0;

    /// Device version/revision number.
    u16 versionNumber = 0;

    /// Returns a 32-character hex string representation of this UUID.
    [[nodiscard]] std::string
    toString() const;
};

} // namespace pomdog
