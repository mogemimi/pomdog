// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameControllerDB;
} // namespace pomdog

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// Loads a GameControllerDB from a FlatBuffers binary file (.gcdb)
/// using the virtual file system.
/// @param fs The file system context used to locate the file.
/// @param filePath The virtual path to the .gcdb file.
/// @return A tuple containing the loaded database and an error object if an error occurred.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<GameControllerDB>, std::unique_ptr<Error>>
loadGameControllerDB(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept;

} // namespace pomdog
