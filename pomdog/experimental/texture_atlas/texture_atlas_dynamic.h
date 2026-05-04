// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Image;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog {

/// TextureAtlasDynamic extends TextureAtlas with runtime sprite packing.
///
/// Call add to register sprites, then build to pack them into a GPU texture.
/// After build returns nullptr, all TextureAtlas lookup methods are available.
class POMDOG_EXPORT TextureAtlasDynamic : public TextureAtlas {
public:
    ~TextureAtlasDynamic() override;

    /// Adds a sprite to the atlas.
    ///
    /// `name` is used as the xxHash32 lookup key at runtime.
    /// When `tightPacking` is true, transparent margins are cropped and the
    /// offsets are stored so the original framing can be reconstructed at draw time.
    virtual void
    add(const std::string& name,
        const std::shared_ptr<Image>& image,
        bool tightPacking = true) = 0;

    /// Packs all added sprites into a GPU texture and returns an error on failure.
    ///
    /// `perTileSpacing` is the gap in pixels between packed sprites, which prevents
    /// colour bleed from bilinear filtering.
    [[nodiscard]] virtual std::unique_ptr<Error>
    build(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        int width,
        int height,
        int perTileSpacing = 2) noexcept = 0;
};

/// Creates an empty `TextureAtlasDynamic` ready for adding sprites and building.
[[nodiscard]] POMDOG_EXPORT std::shared_ptr<TextureAtlasDynamic>
createTextureAtlasDynamic() noexcept;

} // namespace pomdog
