// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::TexturePacker {

/// Loads a TextureAtlas from a .atlas file via VFS.
[[nodiscard]] POMDOG_EXPORT std::tuple<TextureAtlas, std::unique_ptr<Error>>
loadTextureAtlas(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath);

} // namespace pomdog::TexturePacker
