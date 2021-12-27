// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/texture_packer/texture_region.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::TexturePacker {

struct POMDOG_EXPORT TextureAtlasPage final {
    std::string Name;
};

struct POMDOG_EXPORT TextureAtlasRegion final {
    std::string Name;
    TextureRegion Region;
    std::int16_t TexturePage;
};

class POMDOG_EXPORT TextureAtlas final {
public:
    std::vector<TextureAtlasPage> pages;
    std::vector<TextureAtlasRegion> regions;
};

} // namespace pomdog::TexturePacker
