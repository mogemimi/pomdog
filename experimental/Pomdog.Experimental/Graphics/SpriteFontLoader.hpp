//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SPRITEFONTLOADER_30D6FB29_E5E2_407C_B1C9_46B5C3CDE875_HPP
#define POMDOG_SPRITEFONTLOADER_30D6FB29_E5E2_407C_B1C9_46B5C3CDE875_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "SpriteFont.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class AssetManager;

class SpriteFontLoader final {
public:
	static std::shared_ptr<SpriteFont> Load(
		AssetManager & assets, std::string const& assetName);
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEFONTLOADER_30D6FB29_E5E2_407C_B1C9_46B5C3CDE875_HPP)
