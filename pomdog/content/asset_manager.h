// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/asset_builders/builder.h"
#include "pomdog/content/asset_loaders/asset_loader.h"
#include "pomdog/content/asset_loaders/audio_clip_loader.h"
#include "pomdog/content/asset_loaders/texture2d_loader.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <any>
#include <memory>
#include <string>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AudioEngine;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog {

class POMDOG_EXPORT AssetManager final {
private:
    std::string contentDirectory_;
    std::weak_ptr<AudioEngine> audioEngine_;
    std::weak_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::unordered_map<std::string, std::any> assets_;

public:
    AssetManager(
        const std::string& contentDirectory,
        const std::shared_ptr<AudioEngine>& audioEngine,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    /// Loads an asset stored at the specified path in a content directory.
    template <typename T>
    [[nodiscard]] std::tuple<std::shared_ptr<T>, std::unique_ptr<Error>>
    load(const std::string& assetName)
    {
        static_assert(std::is_object<T>::value, "");

        const std::type_index typeIndex = typeid(std::shared_ptr<T>);

        const auto filePath = getAssetPath(assetName);

        auto key = filepaths::toSlash(filePath);
        if (auto iter = assets_.find(key); iter != std::end(assets_)) {
            auto& assetHolder = iter->second;

            if (std::type_index(assetHolder.type()) == typeIndex) {
                auto asset = std::any_cast<std::shared_ptr<T>>(assetHolder);
                return std::make_tuple(std::move(asset), nullptr);
            }

            assets_.erase(iter);
        }

        detail::AssetLoader<T> loader;
        auto [asset, err] = loader(*this, filePath);
        if (err != nullptr) {
            return std::make_tuple(nullptr, std::move(err));
        }

        static_assert(std::is_same<decltype(asset), std::shared_ptr<T>>::value);

        auto assetHolder = std::make_any<std::shared_ptr<T>>(asset);
        POMDOG_ASSERT(std::type_index(assetHolder.type()) == typeIndex);

        assets_.emplace(std::move(key), std::move(assetHolder));

        return std::make_tuple(std::move(asset), nullptr);
    }

    /// Creates an asset builder.
    template <typename T, typename... Arguments>
    [[nodiscard]] AssetBuilders::Builder<T>
    createBuilder(Arguments&&... arguments)
    {
        AssetBuilders::Builder<T> builder(*this, std::forward<Arguments>(arguments)...);
        return builder;
    }

    /// Disposes all loaded assets in this manager.
    void unload();

    /// Gets the path to the asset directory.
    [[nodiscard]] std::string
    getContentDirectory() const noexcept;

    /// Sets the path to the asset directory.
    void setContentDirectory(const std::string& dir) noexcept;

    /// Gets the audio engine for creating audio resources.
    [[nodiscard]] std::shared_ptr<AudioEngine>
    getAudioEngine() const noexcept;

    /// Gets the graphics device for creating graphics resources.
    [[nodiscard]] std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() const noexcept;

    /// Gets the full path to the asset.
    [[nodiscard]] std::string
    getAssetPath(const std::string& assetName) const noexcept;
};

} // namespace pomdog
