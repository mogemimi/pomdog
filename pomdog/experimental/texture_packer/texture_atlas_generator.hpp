// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/experimental/texture_packer/texture_atlas.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
