// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <sys/types.h>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::posix {

/// Creates a directory at `path` with mkdir() and `mode`, which the umask of
/// the process masks. See platformfs::createDirectory().
[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectory(const std::string& path, ::mode_t mode) noexcept;

/// Creates a directory at `path`, along with its missing parent directories,
/// with mkdir() and `mode`. See platformfs::createDirectories().
[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectories(const std::string& path, ::mode_t mode) noexcept;

} // namespace pomdog::detail::posix
