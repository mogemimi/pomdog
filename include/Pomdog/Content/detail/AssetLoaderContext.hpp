// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include <memory>
#include <string>

namespace Pomdog {
namespace Detail {

class POMDOG_EXPORT AssetLoaderContext {
public:
    std::string RootDirectory;
    std::weak_ptr<Pomdog::GraphicsDevice> GraphicsDevice;

    BinaryFileStream OpenStream(std::string const& assetName) const;
};

} // namespace Detail
} // namespace Pomdog
