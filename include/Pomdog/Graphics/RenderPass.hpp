// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstdint>
#include <memory>
#include <tuple>

namespace Pomdog {

using RenderTargetAndClearColor = std::tuple<std::shared_ptr<RenderTarget2D>, Optional<Vector4>>;

struct POMDOG_EXPORT RenderPass {
    std::vector<RenderTargetAndClearColor> RenderTargets;
    Optional<Pomdog::Viewport> Viewport;
    Optional<Rectangle> ScissorRect;
    Optional<float> ClearDepth;
    Optional<std::uint8_t> ClearStencil;
};

} // namespace Pomdog
