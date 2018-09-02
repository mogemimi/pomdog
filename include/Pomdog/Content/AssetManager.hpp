// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Content/detail/AssetDictionary.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT AssetManager final {
public:
    explicit AssetManager(Detail::AssetLoaderContext&& loaderContext);

    template <typename T>
    std::shared_ptr<T> Load(const std::string& assetName)
    {
        return assets.Load<T>(loaderContext, assetName);
    }

    template <typename T, typename... Arguments>
    AssetBuilders::Builder<T> CreateBuilder(Arguments&&... arguments)
    {
        AssetBuilders::Builder<T> builder(loaderContext,
            std::forward<Arguments>(arguments)...);
        return std::move(builder);
    }

    void Unload();

    std::string GetRootDirectory() const;

    Detail::BinaryFileStream OpenStream(const std::string& assetName) const;

private:
    Detail::AssetLoaderContext loaderContext;
    Detail::AssetDictionary assets;
};

} // namespace Pomdog
