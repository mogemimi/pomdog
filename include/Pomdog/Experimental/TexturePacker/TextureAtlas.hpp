// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureRegion.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace Pomdog {
namespace TexturePacker {

struct POMDOG_EXPORT TextureAtlasPage final {
    std::string Name;
};

struct POMDOG_EXPORT TextureAtlasRegion final {
    std::string Name;
    TextureRegion Region;
    std::int16_t TexturePage;
};

class POMDOG_EXPORT TextureAtlas {
public:
    std::vector<TextureAtlasPage> pages;
    std::vector<TextureAtlasRegion> regions;
};

} // namespace TexturePacker
} // namespace Pomdog
