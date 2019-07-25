// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "SkeletonDesc.hpp"

namespace Pomdog {

class AssetManager;

namespace Spine {

class SkeletonDescLoader final {
public:
    static SkeletonDesc Load(AssetManager const& assets, std::string const& assetName);
};

} // namespace Spine
} // namespace Pomdog
