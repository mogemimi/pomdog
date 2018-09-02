// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include <memory>
#include <string>

namespace Pomdog {

class AssetManager;
class SpriteFont;

class POMDOG_EXPORT SpriteFontLoader final {
public:
    static std::shared_ptr<SpriteFont> Load(
        AssetManager& assets, const std::string& assetName);
};

} // namespace Pomdog
