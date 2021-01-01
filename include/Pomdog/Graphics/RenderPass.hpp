// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

using RenderTargetAndClearColor = std::tuple<std::shared_ptr<RenderTarget2D>, std::optional<Vector4>>;

struct POMDOG_EXPORT RenderPass final {
    /// An array of render targets.
    std::array<RenderTargetAndClearColor, 8> RenderTargets;

    /// A depth stencil buffer.
    std::shared_ptr<Pomdog::DepthStencilBuffer> DepthStencilBuffer;

    /// A viewport for projection transformations and clipping.
    std::optional<Pomdog::Viewport> Viewport;

    /// A scissor rectangle for a scissor test.
    std::optional<Rectangle> ScissorRect;

    /// The depth value to use when the depth buffer is cleared.
    std::optional<float> ClearDepth;

    /// The stencil value to use when the stencil buffer is cleared.
    std::optional<std::uint8_t> ClearStencil;
};

} // namespace Pomdog
