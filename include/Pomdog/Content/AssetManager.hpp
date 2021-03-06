// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Content/AssetLoaders/AssetLoader.hpp"
#include "Pomdog/Content/AssetLoaders/AudioClipLoader.hpp"
#include "Pomdog/Content/AssetLoaders/Texture2DLoader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include "Pomdog/Utility/PathHelper.hpp"

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

namespace Pomdog {

class AudioEngine;
class GraphicsDevice;

class POMDOG_EXPORT AssetManager final {
public:
    AssetManager(
        const std::string& contentDirectory,
        const std::shared_ptr<AudioEngine>& audioEngine,
        const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    /// Loads an asset stored at the specified path in a content directory.
    template <typename T>
    [[nodiscard]] std::tuple<std::shared_ptr<T>, std::unique_ptr<Error>>
    Load(const std::string& assetName)
    {
        static_assert(std::is_object<T>::value, "");

        const std::type_index typeIndex = typeid(std::shared_ptr<T>);

        const auto filePath = GetAssetPath(assetName);

        auto key = PathHelper::ToSlash(filePath);
        if (auto iter = assets.find(key); iter != std::end(assets)) {
            auto& assetHolder = iter->second;

            if (std::type_index(assetHolder.type()) == typeIndex) {
                auto asset = std::any_cast<std::shared_ptr<T>>(assetHolder);
                return std::make_tuple(std::move(asset), nullptr);
            }

            assets.erase(iter);
        }

        Detail::AssetLoader<T> loader;
        auto [asset, err] = loader(*this, filePath);
        if (err != nullptr) {
            return std::make_tuple(nullptr, std::move(err));
        }

        static_assert(std::is_same<decltype(asset), std::shared_ptr<T>>::value);

        auto assetHolder = std::make_any<std::shared_ptr<T>>(asset);
        POMDOG_ASSERT(std::type_index(assetHolder.type()) == typeIndex);

        assets.emplace(std::move(key), std::move(assetHolder));

        return std::make_tuple(std::move(asset), nullptr);
    }

    /// Creates an asset builder.
    template <typename T, typename... Arguments>
    [[nodiscard]] AssetBuilders::Builder<T>
    CreateBuilder(Arguments&&... arguments)
    {
        AssetBuilders::Builder<T> builder(*this, std::forward<Arguments>(arguments)...);
        return builder;
    }

    /// Disposes all loaded assets in this manager.
    void Unload();

    /// Gets the path to the asset directory.
    [[nodiscard]] std::string
    GetContentDirectory() const noexcept;

    /// Sets the path to the asset directory.
    void SetContentDirectory(const std::string& dir) noexcept;

    /// Gets the audio engine for creating audio resources.
    [[nodiscard]] std::shared_ptr<AudioEngine>
    GetAudioEngine() const noexcept;

    /// Gets the graphics device for creating graphics resources.
    [[nodiscard]] std::shared_ptr<GraphicsDevice>
    GetGraphicsDevice() const noexcept;

    /// Gets the full path to the asset.
    [[nodiscard]] std::string
    GetAssetPath(const std::string& assetName) const noexcept;

private:
    std::string contentDirectory;
    std::weak_ptr<AudioEngine> audioEngine;
    std::weak_ptr<GraphicsDevice> graphicsDevice;
    std::unordered_map<std::string, std::any> assets;
};

} // namespace Pomdog
