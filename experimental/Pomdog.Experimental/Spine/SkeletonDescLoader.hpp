// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKELETON2DLOADER_24651167_HPP
#define POMDOG_SKELETON2DLOADER_24651167_HPP

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

#endif // POMDOG_SKELETON2DLOADER_24651167_HPP
