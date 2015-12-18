// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "SpriteFont.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include <memory>

namespace Pomdog {

class AssetManager;

class SpriteFontLoader final {
public:
    static std::shared_ptr<SpriteFont> Load(
        AssetManager & assets, std::string const& assetName);
};

} // namespace Pomdog
