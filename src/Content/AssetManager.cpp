//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Content/AssetManager.hpp>

namespace Pomdog {

AssetManager::AssetManager(Details::AssetLoaderContext && loaderContextIn)
	: loaderContext(std::move(loaderContextIn))
{}

}// namespace Pomdog
