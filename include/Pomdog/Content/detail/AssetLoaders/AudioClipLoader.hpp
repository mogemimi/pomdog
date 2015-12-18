// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Content/detail/AssetLoader.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <memory>

namespace Pomdog {

class AudioClip;

namespace Detail {

class AssetLoaderContext;

template <>
struct POMDOG_EXPORT AssetLoader<AudioClip> final {
    std::shared_ptr<AudioClip> operator()(
        AssetLoaderContext const& loaderContext,
        std::string const& assetName);
};

} // namespace Detail
} // namespace Pomdog
