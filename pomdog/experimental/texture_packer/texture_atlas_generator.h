// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Image;
} // namespace pomdog

namespace pomdog::TexturePacker {

struct POMDOG_EXPORT TextureAtlasGeneratorResult final {
    std::shared_ptr<pomdog::Image> Image;
    TextureAtlas Atlas;
    bool HasError;
};

struct POMDOG_EXPORT TextureAtlasGeneratorSource final {
    std::shared_ptr<pomdog::Image> Image;
    std::string Name;
};

class POMDOG_EXPORT TextureAtlasGenerator final {
public:
    static TextureAtlasGeneratorResult Generate(
        const std::vector<TextureAtlasGeneratorSource>& sources,
        int width,
        int height);
};

} // namespace pomdog::TexturePacker
