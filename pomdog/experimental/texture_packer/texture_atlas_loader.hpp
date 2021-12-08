// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/experimental/texture_packer/texture_atlas.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::TexturePacker {

class POMDOG_EXPORT TextureAtlasLoader final {
public:
    [[nodiscard]] static std::tuple<TextureAtlas, std::unique_ptr<Error>>
    Load(const std::string& filePath);
};

} // namespace pomdog::TexturePacker
