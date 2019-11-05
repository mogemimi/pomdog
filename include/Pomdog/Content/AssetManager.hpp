// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Content/AssetLoaders/AssetLoader.hpp"
#include "Pomdog/Content/AssetLoaders/AudioClipLoader.hpp"
#include "Pomdog/Content/AssetLoaders/Texture2DLoader.hpp"
#include "Pomdog/Utility/Any.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <memory>
#include <string>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace Pomdog {

class GraphicsDevice;

class POMDOG_EXPORT AssetManager final {
public:
    AssetManager(
        const std::string& contentDirectory,
        const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    /// Loads an asset stored at the specified path in a content directory.
    template <typename T>
    [[nodiscard]] std::tuple<std::shared_ptr<T>, std::shared_ptr<Error>>
    Load(const std::string& assetName)
    {
        static_assert(std::is_object<T>::value, "");

        const std::type_index typeIndex = typeid(std::shared_ptr<T>);

        const auto filePath = GetAssetPath(assetName);

        auto key = PathHelper::ToSlash(filePath);
        if (auto iter = assets.find(key); iter != std::end(assets)) {
            auto& assetHolder = iter->second;

            if (assetHolder.Type() == typeIndex) {
                auto asset = assetHolder.As<std::shared_ptr<T>>();
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

        Any assetHolder = asset;
        POMDOG_ASSERT(assetHolder.Type() == typeIndex);

        assets.emplace(std::move(key), std::move(assetHolder));

        return std::make_tuple(std::move(asset), nullptr);
    }

    /// Creates an asset builder.
    template <typename T, typename... Arguments>
    AssetBuilders::Builder<T> CreateBuilder(Arguments&&... arguments)
    {
        AssetBuilders::Builder<T> builder(*this,
            std::forward<Arguments>(arguments)...);
        return std::move(builder);
    }

    /// Disposes all loaded assets in this manager.
    void Unload();

    /// Gets the path to the asset directory.
    std::string GetContentDirectory() const;

    /// Sets the path to the asset directory.
    void SetContentDirectory(const std::string& dir);

    /// Gets the graphics device for creating graphics resources.
    std::shared_ptr<GraphicsDevice> GetGraphicsDevice() const;

    /// Gets the full path to the asset.
    std::string GetAssetPath(const std::string& assetName) const;

private:
    std::string contentDirectory;
    std::weak_ptr<GraphicsDevice> graphicsDevice;
    std::unordered_map<std::string, Any> assets;
};

} // namespace Pomdog
