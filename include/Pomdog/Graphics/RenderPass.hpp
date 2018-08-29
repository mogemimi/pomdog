// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <tuple>

namespace Pomdog {

using RenderTargetAndClearColor = std::tuple<std::shared_ptr<RenderTarget2D>, std::optional<Vector4>>;

struct POMDOG_EXPORT RenderPass final {
    std::vector<RenderTargetAndClearColor> RenderTargets;
    std::optional<Pomdog::Viewport> Viewport;
    std::optional<Rectangle> ScissorRect;
    std::optional<float> ClearDepth;
    std::optional<std::uint8_t> ClearStencil;
};

} // namespace Pomdog
