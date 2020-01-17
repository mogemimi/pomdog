// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog {
class AssetManager;
class SpriteFont;
} // namespace Pomdog

namespace Pomdog::Detail {

template <>
struct POMDOG_EXPORT AssetLoader<SpriteFont> final {
    [[nodiscard]] std::tuple<std::shared_ptr<SpriteFont>, std::shared_ptr<Error>>
    operator()(AssetManager& assets, const std::string& filePath);
};

} // namespace Pomdog::Detail
