// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP
#define POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "AssetLoaders/AudioClipLoader.hpp"
#include "AssetLoaders/Texture2DLoader.hpp"
#include "Pomdog/Utility/detail/Any.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <string>
#include <memory>

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
    std::shared_ptr<T> Load(AssetLoaderContext const& loaderContext, std::string const& assetName)
    {
        static_assert(std::is_object<T>::value, "");

        std::type_index const typeIndex = typeid(std::shared_ptr<T>);

        auto const iter = assets.find(assetName);
        if (iter != std::end(assets))
        {
            auto & assetHolder = iter->second;

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

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP)
