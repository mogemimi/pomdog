// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
class MountVolume;
} // namespace pomdog::vfs

namespace pomdog::vfs {

/// Opens an archive (.idx + .pak) as a memory-mapped MountVolume.
///
/// The .idx and .pak files are memory-mapped for efficient access.
/// This function is only available on platforms that support memory mapping
/// (Windows, Linux, macOS). It is excluded from Emscripten builds.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
openArchiveFileMmap(const std::string& idxPath, const std::string& pakPath) noexcept;

} // namespace pomdog::vfs
