// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog.Experimental/TexturePacker/TextureAtlasLoader.hpp"

namespace TestApp {

using namespace Pomdog;

void LogTexturePackerInfo(TexturePacker::TextureAtlas const& textureAtlas);

void LogSkeletalInfo(Spine::SkeletonDesc const& skeletonDesc);

}// namespace TestApp
