// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/texture_atlas/texture_region.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class Texture2D;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// TextureAtlas provides access to a packed sprite texture and its region metadata.
///
/// Use `createTextureAtlas` to load a pre-built atlas from a FlatBuffers `.tileset`
/// binary produced offline by `png2atlas`, `piskel2atlas` or `libgdx-atlas-to-tileset`.
/// Use `createTextureAtlasDynamic` to build an atlas at runtime from Image objects.
class POMDOG_EXPORT TextureAtlas {
public:
    virtual ~TextureAtlas();

    /// Returns the GPU texture that holds all packed sprites.
    [[nodiscard]] virtual std::shared_ptr<gpu::Texture2D>
    getTexture() const noexcept = 0;

    /// Returns the region at the given index (direct access, O(1)).
    [[nodiscard]] virtual TextureRegion
    getRegion(u32 index) const noexcept = 0;

    /// Returns the region for the given xxHash32 key (binary search, O(log n)).
    /// Returns an empty region if the key is not found.
    [[nodiscard]] virtual TextureRegion
    getRegionByKey(u32 key) const noexcept = 0;

    /// Returns the index of the region with the given key, or nullopt if not found.
    [[nodiscard]] virtual std::optional<u32>
    findRegionByKey(u32 key) const noexcept = 0;

    /// Returns true if the atlas texture uses premultiplied alpha.
    [[nodiscard]] virtual bool
    isPremultipliedAlpha() const noexcept = 0;
};

/// Creates a TextureAtlas by loading the FlatBuffers `.tileset` binary at tilesetPath
/// and the packed atlas texture at texturePath.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<TextureAtlas>, std::unique_ptr<Error>>
createTextureAtlas(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::string_view tilesetPath,
    std::string_view texturePath) noexcept;

/// Creates a TextureAtlas using xxHash-64 file keys for zero-string lookup in a packaged archive.
/// mountVolumeHashKey is the xxHash-64 of the mount point's virtual path
/// (e.g. computeStringHash64("/assets")).
/// tilesetHashKey is the xxHash-64 of the tileset file path relative to the mount point
/// (e.g. computeStringHash64("textures/sprites.tileset")).
/// textureHashKey is the xxHash-64 of the texture file path relative to the mount point
/// (e.g. computeStringHash64("textures/sprites.png")).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<TextureAtlas>, std::unique_ptr<Error>>
createTextureAtlas(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    u64 mountVolumeHashKey,
    u64 tilesetHashKey,
    u64 textureHashKey) noexcept;

} // namespace pomdog
