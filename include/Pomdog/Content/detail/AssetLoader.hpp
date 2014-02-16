//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ASSETLOADER_085F106A_3B25_4AAC_BFE8_2C9AFD3C623C_HPP
#define POMDOG_ASSETLOADER_085F106A_3B25_4AAC_BFE8_2C9AFD3C623C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <string>

namespace Pomdog {

class GraphicsDevice;

namespace Details {

template <typename T>
struct AssetLoader;

struct AssetLoaderContext
{
	std::string rootDirectory;
	std::shared_ptr<GraphicsDevice> graphicsDevice;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ASSETLOADER_085F106A_3B25_4AAC_BFE8_2C9AFD3C623C_HPP)
