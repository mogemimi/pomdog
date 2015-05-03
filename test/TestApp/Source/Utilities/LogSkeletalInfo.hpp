// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LOGSKELETALINFO_7178CCE1_HPP
#define POMDOG_LOGSKELETALINFO_7178CCE1_HPP

#include "Pomdog.Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog.Experimental/TexturePacker/TextureAtlasLoader.hpp"

namespace TestApp {

using namespace Pomdog;

void LogTexturePackerInfo(TexturePacker::TextureAtlas const& textureAtlas);

void LogSkeletalInfo(Spine::SkeletonDesc const& skeletonDesc);

}// namespace TestApp

#endif // POMDOG_LOGSKELETALINFO_7178CCE1_HPP
