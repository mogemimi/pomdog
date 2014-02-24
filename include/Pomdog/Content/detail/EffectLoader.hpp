//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTLOADER_868B2144_20B3_4B9C_A8FD_F027A525455E_HPP
#define POMDOG_EFFECTLOADER_868B2144_20B3_4B9C_A8FD_F027A525455E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <string>
#include "../../Config/Export.hpp"
#include "AssetLoader.hpp"

namespace Pomdog {

class EffectPass;

namespace Details {

// explicit instantiations
template <>
std::shared_ptr<EffectPass> POMDOG_EXPORT
AssetLoader<EffectPass>::operator()(AssetLoaderContext const& loaderContext,
	std::string const& assetPath);

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTLOADER_868B2144_20B3_4B9C_A8FD_F027A525455E_HPP)
