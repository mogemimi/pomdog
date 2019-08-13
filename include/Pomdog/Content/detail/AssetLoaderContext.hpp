// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <string>

namespace Pomdog::Detail {

class POMDOG_EXPORT AssetLoaderContext final {
public:
    std::string RootDirectory;
    std::weak_ptr<Pomdog::GraphicsDevice> GraphicsDevice;

    BinaryFileStream OpenStream(const std::string& assetName) const;
};

} // namespace Pomdog::Detail
