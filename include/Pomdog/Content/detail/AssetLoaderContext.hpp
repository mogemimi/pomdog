// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ASSETLOADERCONTEXT_E4996CA2_HPP
#define POMDOG_ASSETLOADERCONTEXT_E4996CA2_HPP

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

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_ASSETLOADERCONTEXT_E4996CA2_HPP
