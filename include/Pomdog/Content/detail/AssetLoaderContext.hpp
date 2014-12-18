//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ASSETLOADERCONTEXT_E4996CA2_38E7_45F5_BBED_9DC401425E70_HPP
#define POMDOG_ASSETLOADERCONTEXT_E4996CA2_38E7_45F5_BBED_9DC401425E70_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Config/Export.hpp"
#include <memory>
#include <string>
#include <fstream>

namespace Pomdog {

class GraphicsDevice;

namespace Details {

class POMDOG_EXPORT AssetLoaderContext {
public:
	std::string RootDirectory;
	std::weak_ptr<Pomdog::GraphicsDevice> GraphicsDevice;
	
	std::ifstream OpenStream(std::string const& assetName) const;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ASSETLOADERCONTEXT_E4996CA2_38E7_45F5_BBED_9DC401425E70_HPP)
