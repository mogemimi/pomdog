//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EFFECTPASSLOADER_C9744EEA_2F5B_46D6_8880_B11D2DF37382_HPP
#define POMDOG_EFFECTPASSLOADER_C9744EEA_2F5B_46D6_8880_B11D2DF37382_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Content/detail/AssetLoader.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <memory>

namespace Pomdog {

class EffectPass;

namespace Details {

class AssetLoaderContext;

template <>
struct POMDOG_EXPORT AssetLoader<EffectPass> final {
	std::shared_ptr<EffectPass> operator()(
		AssetLoaderContext const& loaderContext,
		std::string const& assetName);
};

}// namespace Details
}// namespace Pomdogs

#endif // !defined(POMDOG_EFFECTPASSLOADER_C9744EEA_2F5B_46D6_8880_B11D2DF37382_HPP)
