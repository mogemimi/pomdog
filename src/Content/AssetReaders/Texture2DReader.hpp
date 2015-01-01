//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TEXTURE2DREADER_883B68A7_B64F_4C22_A88F_D764B0CCF37A_HPP
#define POMDOG_TEXTURE2DREADER_883B68A7_B64F_4C22_A88F_D764B0CCF37A_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <string>
#include <memory>

namespace Pomdog {

class Texture2D;

namespace Details {

class AssetLoaderContext;

class Texture2DReader final {
public:
	static std::shared_ptr<Texture2D> Read(
		AssetLoaderContext const& loaderContext, std::string const& assetName);
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTURE2DREADER_883B68A7_B64F_4C22_A88F_D764B0CCF37A_HPP)
