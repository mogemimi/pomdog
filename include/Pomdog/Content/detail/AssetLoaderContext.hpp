// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ASSETLOADERCONTEXT_E4996CA2_38E7_45F5_BBED_9DC401425E70_HPP
#define POMDOG_ASSETLOADERCONTEXT_E4996CA2_38E7_45F5_BBED_9DC401425E70_HPP

#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <string>
#include <fstream>

namespace Pomdog {

class GraphicsDevice;

namespace Detail {

class POMDOG_EXPORT AssetLoaderContext {
public:
    std::string RootDirectory;
    std::weak_ptr<Pomdog::GraphicsDevice> GraphicsDevice;

    std::ifstream OpenStream(std::string const& assetName) const;
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_ASSETLOADERCONTEXT_E4996CA2_38E7_45F5_BBED_9DC401425E70_HPP)
