// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/detail/AssetDictionary.hpp"

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
AssetDictionary::AssetDictionary() = default;
//-----------------------------------------------------------------------
AssetDictionary::~AssetDictionary() = default;
//-----------------------------------------------------------------------
void AssetDictionary::Unload()
{
    assets.clear();
}
//-----------------------------------------------------------------------
}// namespace Detail
}// namespace Pomdog
