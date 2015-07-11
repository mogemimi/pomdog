// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ASSETMANAGER_FAF5E4A4_HPP
#define POMDOG_ASSETMANAGER_FAF5E4A4_HPP

#include "detail/AssetLoaderContext.hpp"
#include "detail/AssetDictionary.hpp"
#include "AssetBuilders/Builder.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT AssetManager {
public:
    explicit AssetManager(Detail::AssetLoaderContext && loaderContext);

    template <typename T>
    std::shared_ptr<T> Load(std::string const& assetName)
    {
        return assets.Load<T>(loaderContext, assetName);
    }

    template <typename T, typename ...Arguments>
    AssetBuilders::Builder<T> CreateBuilder(Arguments && ...arguments)
    {
        AssetBuilders::Builder<T> builder(loaderContext,
            std::forward<Arguments>(arguments)...);
        return std::move(builder);
    }

    void Unload();

    std::string RootDirectory() const;

    Detail::BinaryFileStream OpenStream(std::string const& assetName) const;

private:
    Detail::AssetLoaderContext loaderContext;
    Detail::AssetDictionary assets;
};

} // namespace Pomdog

#endif // POMDOG_ASSETMANAGER_FAF5E4A4_HPP
