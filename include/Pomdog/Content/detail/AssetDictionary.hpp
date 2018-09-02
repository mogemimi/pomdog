// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "AssetLoaders/Texture2DLoader.hpp"
#include "Pomdog/Content/detail/AssetLoaders/AudioClipLoader.hpp"
#include "Pomdog/Utility/Any.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace Pomdog {
namespace Detail {

class AssetLoaderContext;

class POMDOG_EXPORT AssetDictionary final {
private:
    std::unordered_map<std::string, Any> assets;

public:
    AssetDictionary();

    ~AssetDictionary();

    template <typename T>
    std::shared_ptr<T> Load(const AssetLoaderContext& loaderContext, const std::string& assetName)
    {
        static_assert(std::is_object<T>::value, "");

        std::type_index const typeIndex = typeid(std::shared_ptr<T>);

        auto const iter = assets.find(assetName);
        if (iter != std::end(assets)) {
            auto& assetHolder = iter->second;

            if (assetHolder.Type() == typeIndex) {
                return assetHolder.As<std::shared_ptr<T>>();
            }

            assets.erase(iter);
        }

        AssetLoader<T> loader;
        auto asset = loader(loaderContext, assetName);

        static_assert(std::is_same<decltype(asset), std::shared_ptr<T>>::value, "");

        Any assetHolder = asset;
        POMDOG_ASSERT(assetHolder.Type() == typeIndex);

        assets.emplace(assetName, std::move(assetHolder));

        return std::move(asset);
    }

    void Unload();
};

} // namespace Detail
} // namespace Pomdog
