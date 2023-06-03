// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/truetype_font_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/graphics/truetype_font.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

std::tuple<std::shared_ptr<TrueTypeFont>, std::unique_ptr<Error>>
AssetLoader<TrueTypeFont>::operator()([[maybe_unused]] AssetManager& assets, const std::string& filePath)
{
    auto font = std::make_shared<TrueTypeFont>();

    if (auto err = font->load(filePath); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(font), nullptr);
}

} // namespace pomdog::detail
