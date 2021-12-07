// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/content/asset_loaders/asset_loader.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class AssetManager;
class Texture2D;
} // namespace Pomdog

namespace Pomdog::Detail {

template <>
struct POMDOG_EXPORT AssetLoader<Texture2D> final {
    [[nodiscard]] std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
    operator()(AssetManager& assets, const std::string& filePath);
};

} // namespace Pomdog::Detail
