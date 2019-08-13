// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/detail/AssetDictionary.hpp"

namespace Pomdog::Detail {

AssetDictionary::AssetDictionary() = default;

AssetDictionary::~AssetDictionary() = default;

void AssetDictionary::Unload()
{
    assets.clear();
}

} // namespace Pomdog::Detail
