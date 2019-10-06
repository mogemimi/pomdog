// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetLoaders/AssetLoader.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog {
class AssetManager;
} // namespace Pomdog

namespace Pomdog::MagicaVoxel {
class VoxModel;
} // namespace Pomdog::MagicaVoxel

namespace Pomdog::Detail {

template <>
struct POMDOG_EXPORT AssetLoader<MagicaVoxel::VoxModel> final {
    [[nodiscard]] std::tuple<std::shared_ptr<MagicaVoxel::VoxModel>, std::shared_ptr<Error>>
    operator()(AssetManager& assets, const std::string& filePath);
};

} // namespace Pomdog::Detail
