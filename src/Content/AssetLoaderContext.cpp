// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <utility>

namespace Pomdog::Detail {

BinaryFileStream AssetLoaderContext::OpenStream(const std::string& assetName) const
{
    auto filePath = PathHelper::Join(RootDirectory, assetName);
    return PathHelper::OpenStream(filePath);
}

} // namespace Pomdog::Detail
