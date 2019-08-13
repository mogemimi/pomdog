// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Pomdog {
class Image;
} // namespace Pomdog

namespace Pomdog::TexturePacker {

struct POMDOG_EXPORT TextureAtlasGeneratorResult final {
    std::shared_ptr<Pomdog::Image> Image;
    TextureAtlas Atlas;
    bool HasError;
};

struct POMDOG_EXPORT TextureAtlasGeneratorSource final {
    std::shared_ptr<Pomdog::Image> Image;
    std::string Name;
};

class POMDOG_EXPORT TextureAtlasGenerator final {
public:
    static TextureAtlasGeneratorResult Generate(
        const std::vector<TextureAtlasGeneratorSource>& sources,
        int width,
        int height);
};

} // namespace Pomdog::TexturePacker
