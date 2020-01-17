// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetLoaders/AssetLoader.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog {
class AssetManager;
class Texture2D;
} // namespace Pomdog

namespace Pomdog::Detail {

template <>
struct POMDOG_EXPORT AssetLoader<Texture2D> final {
    [[nodiscard]] std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
    operator()(AssetManager& assets, const std::string& filePath);
};

} // namespace Pomdog::Detail
