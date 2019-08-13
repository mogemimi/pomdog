// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/detail/AssetLoader.hpp"
#include <memory>
#include <string>

namespace Pomdog {
class AudioClip;
} // namespace Pomdog

namespace Pomdog::Detail {

class AssetLoaderContext;

template <>
struct POMDOG_EXPORT AssetLoader<AudioClip> final {
    std::shared_ptr<AudioClip> operator()(
        const AssetLoaderContext& loaderContext,
        const std::string& assetName);
};

} // namespace Pomdog::Detail
