// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTURE2DLOADER_84BCFDDA_HPP
#define POMDOG_TEXTURE2DLOADER_84BCFDDA_HPP

#include "Pomdog/Content/detail/AssetLoader.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <memory>

namespace Pomdog {

class Texture2D;

namespace Detail {

class AssetLoaderContext;

template <>
struct POMDOG_EXPORT AssetLoader<Texture2D> final {
    std::shared_ptr<Texture2D> operator()(
        AssetLoaderContext const& loaderContext,
        std::string const& assetName);
};

}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_TEXTURE2DLOADER_84BCFDDA_HPP
