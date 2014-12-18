//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EFFECTPASSREADER_52739AE6_6571_4209_BF92_B3EE344C141D_HPP
#define POMDOG_EFFECTPASSREADER_52739AE6_6571_4209_BF92_B3EE344C141D_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <string>
#include <memory>

namespace Pomdog {

class EffectPass;

namespace Details {

class AssetLoaderContext;

class EffectPassReader final {
public:
	static std::shared_ptr<EffectPass> Read(
		AssetLoaderContext const& loaderContext, std::string const& assetName);
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASSREADER_52739AE6_6571_4209_BF92_B3EE344C141D_HPP)
