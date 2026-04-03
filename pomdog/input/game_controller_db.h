// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
struct GamepadUUID;
} // namespace pomdog

namespace pomdog {

/// GameControllerDB provides a mapping database that translates device UUIDs
/// into button/axis layouts. Implementations are backed by either a FlatBuffers
/// binary (.gcdb) or a platform-specific default mapping table.
class POMDOG_EXPORT GameControllerDB {
public:
    GameControllerDB() noexcept;

    virtual ~GameControllerDB() noexcept;

    GameControllerDB(const GameControllerDB&) = delete;
    GameControllerDB& operator=(const GameControllerDB&) = delete;
    GameControllerDB(GameControllerDB&&) = delete;
    GameControllerDB& operator=(GameControllerDB&&) = delete;

    /// Looks up the button/axis mappings for the given gamepad UUID.
    /// Returns the per-device mapping data and the device name.
    /// If no matching entry is found, platform-specific defaults are returned.
    [[nodiscard]] virtual std::tuple<detail::GamepadMappingEntry, std::string>
    getMappings(const GamepadUUID& uuid) const noexcept = 0;

    /// Looks up the button/axis mappings for the given UUID string.
    [[nodiscard]] virtual std::tuple<detail::GamepadMappingEntry, std::string>
    getMappings(const std::string& uuidString) const noexcept = 0;
};

/// Creates a GameControllerDB backed by a FlatBuffers binary (.gcdb).
/// The storage must contain a valid GameControllerDB FlatBuffers table.
/// Ownership of the storage is transferred to the returned object.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<GameControllerDB>, std::unique_ptr<Error>>
createGameControllerDB(std::unique_ptr<u8[]> storage, std::size_t size);

/// Creates a GameControllerDB that returns only platform-specific defaults.
/// No database lookup is performed; the device name is always empty.
[[nodiscard]] POMDOG_EXPORT std::shared_ptr<GameControllerDB>
createGameControllerDBDummy();

} // namespace pomdog
