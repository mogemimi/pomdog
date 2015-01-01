//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Content/detail/AssetDictionary.hpp"

namespace Pomdog {
namespace Details {
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
}// namespace Details
}// namespace Pomdog
