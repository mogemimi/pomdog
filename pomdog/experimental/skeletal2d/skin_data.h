// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {
class File;
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdogschemas {
struct Skeletal2DSkin;
struct Skeletal2DSkinSet;
} // namespace pomdogschemas

namespace pomdog::skeletal2d {

/// SkinData is the skins of a `.skin_skel2d` file, which
/// `spine-json-to-skeletal2d` converts offline from a Spine JSON file.
///
/// It keeps the file open and reads the FlatBuffers tables from its view, so
/// that a memory-mapped archive serves the skins without copying them.
///
/// The skins refer to joints by index, so use them with the skeleton file
/// that was converted from the same source.
class SkinData final {
private:
    std::shared_ptr<vfs::File> file_;
    const pomdogschemas::Skeletal2DSkinSet* table_ = nullptr;

public:
    /// Takes a table in the view of `file` that `loadSkinData()` has verified.
    SkinData(
        std::shared_ptr<vfs::File> file,
        const pomdogschemas::Skeletal2DSkinSet* table) noexcept;

    /// Returns the skin whose name has the given xxHash32 key, or nullptr when
    /// the file has no such skin.
    ///
    /// The skin is verified. Every slot follows a joint and refers to a mesh
    /// of the skin or to none, and every mesh has vertices that follow a joint
    /// and triangles whose indices refer to its vertices.
    [[nodiscard]] const pomdogschemas::Skeletal2DSkin*
    findSkin(u32 key) const noexcept;
};

/// Loads a `.skin_skel2d` file and verifies its content.
[[nodiscard]] std::tuple<std::shared_ptr<SkinData>, std::unique_ptr<Error>>
loadSkinData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept;

} // namespace pomdog::skeletal2d
