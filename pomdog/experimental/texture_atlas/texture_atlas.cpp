// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/content/texture_loader.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/texture_atlas_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <span>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

class TextureAtlasImpl final : public TextureAtlas {
private:
    std::shared_ptr<gpu::Texture2D> texture_;
    std::unique_ptr<u8[]> storage_;

public:
    TextureAtlasImpl() = default;

    TextureAtlasImpl(const TextureAtlasImpl&) = delete;
    TextureAtlasImpl& operator=(const TextureAtlasImpl&) = delete;

    [[nodiscard]] std::unique_ptr<Error>
    loadTileset(std::shared_ptr<vfs::File> file, std::string_view contextName) noexcept
    {
        auto [stat, statErr] = file->stat();
        if (statErr != nullptr) {
            return errors::wrap(std::move(statErr), "stat failed: " + std::string{contextName});
        }

        if (stat.size <= 0) {
            return errors::make("file is empty: " + std::string{contextName});
        }

        storage_ = std::make_unique<u8[]>(stat.size);
        auto [bytesRead, readErr] = file->read(std::span<u8>{storage_.get(), static_cast<std::size_t>(stat.size)});
        if (readErr != nullptr) {
            return errors::wrap(std::move(readErr), "read failed: " + std::string{contextName});
        }

        if (auto err = file->close(); err != nullptr) {
            return errors::wrap(std::move(err), "close failed: " + std::string{contextName});
        }

        flatbuffers::Verifier verifier(storage_.get(), static_cast<std::size_t>(stat.size));
        if (!pomdogschemas::VerifyTextureAtlasBuffer(verifier)) {
            return errors::make("FlatBuffers verification failed: " + std::string{contextName});
        }

        {
            const auto* atlas = pomdogschemas::GetTextureAtlas(storage_.get());
            if (atlas == nullptr) {
                return errors::make("failed to get TextureAtlas from buffer: " + std::string{contextName});
            }
            if (atlas->keys() == nullptr) {
                return errors::make("TextureAtlas has no keys: " + std::string{contextName});
            }
            if (atlas->regions() == nullptr) {
                return errors::make("TextureAtlas has no regions: " + std::string{contextName});
            }
            if (atlas->keys()->size() != atlas->regions()->size()) {
                return errors::make("TextureAtlas keys/regions size mismatch: " + std::string{contextName});
            }
        }

        return nullptr;
    }

    [[nodiscard]] std::unique_ptr<Error>
    load(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::string_view tilesetPath,
        std::string_view texturePath) noexcept
    {
        {
            auto [tex, loadErr] = loadTexture2D(fs, graphicsDevice, texturePath);
            if (loadErr != nullptr) {
                return errors::wrap(std::move(loadErr), "failed to load atlas texture");
            }
            texture_ = std::move(tex);
        }

        auto [file, openErr] = vfs::open(fs, std::string{tilesetPath});
        if (openErr != nullptr) {
            return errors::wrap(std::move(openErr), "cannot open: " + std::string{tilesetPath});
        }

        return loadTileset(std::move(file), tilesetPath);
    }

    [[nodiscard]] std::unique_ptr<Error>
    loadByHash(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        u64 mountVolumeHashKey,
        u64 tilesetHashKey,
        u64 textureHashKey) noexcept
    {
        {
            auto [tex, loadErr] = loadTexture2D(fs, graphicsDevice, mountVolumeHashKey, textureHashKey);
            if (loadErr != nullptr) {
                return errors::wrap(std::move(loadErr), "failed to load atlas texture by hash");
            }
            texture_ = std::move(tex);
        }

        auto [file, openErr] = vfs::open(fs, mountVolumeHashKey, tilesetHashKey);
        if (openErr != nullptr) {
            return errors::wrap(std::move(openErr), "cannot open tileset by hash key");
        }

        return loadTileset(std::move(file), "(hash-keyed tileset)");
    }

    [[nodiscard]] std::shared_ptr<gpu::Texture2D>
    getTexture() const noexcept override
    {
        return texture_;
    }

    [[nodiscard]] TextureRegion
    getRegion(u32 index) const noexcept override
    {
        const auto* atlas = pomdogschemas::GetTextureAtlas(storage_.get());
        POMDOG_ASSERT(atlas != nullptr);

        const auto* regions = atlas->regions();
        POMDOG_ASSERT(regions != nullptr);
        POMDOG_ASSERT(index < static_cast<u32>(regions->size()));

        const auto* t = regions->Get(index);
        POMDOG_ASSERT(t != nullptr);

        return TextureRegion{
            .subrectX = t->subrect_x(),
            .subrectY = t->subrect_y(),
            .subrectWidth = t->subrect_width(),
            .subrectHeight = t->subrect_height(),
            .xOffset = t->x_offset(),
            .yOffset = t->y_offset(),
            .width = t->width(),
            .height = t->height(),
        };
    }

    [[nodiscard]] TextureRegion
    getRegionByKey(u32 key) const noexcept override
    {
        const auto* atlas = pomdogschemas::GetTextureAtlas(storage_.get());
        POMDOG_ASSERT(atlas != nullptr);

        const auto* keys = atlas->keys();
        POMDOG_ASSERT(keys != nullptr);

        const auto iter = std::lower_bound(
            keys->begin(),
            keys->end(),
            key,
            [](u32 a, u32 k) -> bool { return a < k; });

        if (iter == keys->end() || *iter != key) {
            return TextureRegion{};
        }

        const auto index = std::distance(keys->begin(), iter);
        POMDOG_ASSERT(index >= 0);
        POMDOG_ASSERT(index < static_cast<std::ptrdiff_t>(keys->size()));

        const auto* regions = atlas->regions();
        POMDOG_ASSERT(regions != nullptr);
        POMDOG_ASSERT(index < static_cast<std::ptrdiff_t>(regions->size()));

        const auto* t = regions->Get(static_cast<u32>(index));
        POMDOG_ASSERT(t != nullptr);

        return TextureRegion{
            .subrectX = t->subrect_x(),
            .subrectY = t->subrect_y(),
            .subrectWidth = t->subrect_width(),
            .subrectHeight = t->subrect_height(),
            .xOffset = t->x_offset(),
            .yOffset = t->y_offset(),
            .width = t->width(),
            .height = t->height(),
        };
    }

    [[nodiscard]] std::optional<u32>
    findRegionByKey(u32 key) const noexcept override
    {
        const auto* atlas = pomdogschemas::GetTextureAtlas(storage_.get());
        POMDOG_ASSERT(atlas != nullptr);

        const auto* keys = atlas->keys();
        POMDOG_ASSERT(keys != nullptr);

        const auto iter = std::lower_bound(
            keys->begin(),
            keys->end(),
            key,
            [](u32 a, u32 k) -> bool { return a < k; });

        if (iter == keys->end() || *iter != key) {
            return std::nullopt;
        }

        const auto index = std::distance(keys->begin(), iter);
        POMDOG_ASSERT(index >= 0);
        POMDOG_ASSERT(index < static_cast<std::ptrdiff_t>(keys->size()));

        return static_cast<u32>(index);
    }

    [[nodiscard]] bool
    isPremultipliedAlpha() const noexcept override
    {
        const auto* atlas = pomdogschemas::GetTextureAtlas(storage_.get());
        POMDOG_ASSERT(atlas != nullptr);

        return atlas->premultiplied_alpha();
    }
};

} // namespace

TextureAtlas::~TextureAtlas() = default;

[[nodiscard]] std::tuple<std::shared_ptr<TextureAtlas>, std::unique_ptr<Error>>
createTextureAtlas(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::string_view tilesetPath,
    std::string_view texturePath) noexcept
{
    auto impl = std::make_shared<TextureAtlasImpl>();
    if (auto err = impl->load(fs, graphicsDevice, tilesetPath, texturePath); err != nullptr) {
        return {nullptr, std::move(err)};
    }
    return {std::move(impl), nullptr};
}

[[nodiscard]] std::tuple<std::shared_ptr<TextureAtlas>, std::unique_ptr<Error>>
createTextureAtlas(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    u64 mountVolumeHashKey,
    u64 tilesetHashKey,
    u64 textureHashKey) noexcept
{
    auto impl = std::make_shared<TextureAtlasImpl>();
    if (auto err = impl->loadByHash(fs, graphicsDevice, mountVolumeHashKey, tilesetHashKey, textureHashKey); err != nullptr) {
        return {nullptr, std::move(err)};
    }
    return {std::move(impl), nullptr};
}

} // namespace pomdog
