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

namespace pomdog {
class AssetManager;
} // namespace pomdog

namespace pomdog::magicavoxel {
class VoxModel;
} // namespace pomdog::magicavoxel

namespace pomdog::detail {

template <>
struct POMDOG_EXPORT AssetLoader<magicavoxel::VoxModel> final {
    [[nodiscard]] std::tuple<std::shared_ptr<magicavoxel::VoxModel>, std::unique_ptr<Error>>
    operator()(AssetManager& assets, const std::string& filePath);
};

} // namespace pomdog::detail
