// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/asset_loaders/asset_loader.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
class TrueTypeFont;
} // namespace pomdog

namespace pomdog::detail {

template <>
struct POMDOG_EXPORT AssetLoader<TrueTypeFont> final {
    [[nodiscard]] std::tuple<std::shared_ptr<TrueTypeFont>, std::unique_ptr<Error>>
    operator()(AssetManager& assets, const std::string& filePath);
};

} // namespace pomdog::detail
