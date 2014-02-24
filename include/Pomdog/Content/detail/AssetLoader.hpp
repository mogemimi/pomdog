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

class GraphicsContext;
class GraphicsDevice;

namespace Details {

struct AssetLoaderContext;

template <typename T>
struct AssetLoader
{
	std::shared_ptr<T> operator()(AssetLoaderContext const& loaderContext,
		std::string const& assetPath);
};

struct AssetLoaderContext
{
	std::string RootDirectory;
	std::weak_ptr<Pomdog::GraphicsContext> GraphicsContext;
	std::weak_ptr<Pomdog::GraphicsDevice> GraphicsDevice;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ASSETLOADER_085F106A_3B25_4AAC_BFE8_2C9AFD3C623C_HPP)
