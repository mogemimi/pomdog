//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_LOGSKELETALINFO_7178CCE1_65C2_4CB5_96CD_AB53B07E4601_HPP
#define POMDOG_LOGSKELETALINFO_7178CCE1_65C2_4CB5_96CD_AB53B07E4601_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog.Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog.Experimental/TexturePacker/TextureAtlasLoader.hpp"

namespace TestApp {

using namespace Pomdog;

void LogTexturePackerInfo(TexturePacker::TextureAtlas const& textureAtlas);

void LogSkeletalInfo(Spine::SkeletonDesc const& skeletonDesc);

}// namespace TestApp

#endif // !defined(POMDOG_LOGSKELETALINFO_7178CCE1_65C2_4CB5_96CD_AB53B07E4601_HPP)
