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

namespace pomdog {
class ParticleClip;
} // namespace pomdog

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// Loads a ParticleClip from a JSON file via VFS.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
loadParticleClip(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath);

} // namespace pomdog
