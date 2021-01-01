// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::TexturePacker {

class POMDOG_EXPORT TextureAtlasLoader final {
public:
    [[nodiscard]] static std::tuple<TextureAtlas, std::unique_ptr<Error>>
    Load(const std::string& filePath);
};

} // namespace Pomdog::TexturePacker
