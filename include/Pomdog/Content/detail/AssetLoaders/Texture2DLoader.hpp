// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

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
        const AssetLoaderContext& loaderContext,
        const std::string& assetName);
};

} // namespace Detail
} // namespace Pomdog
