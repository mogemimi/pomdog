// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/depth_stencil_operation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct POMDOG_EXPORT DepthStencilDesc final {
    DepthStencilOperation clockwiseFace = {};
    DepthStencilOperation counterClockwiseFace = {};
    i32 referenceStencil = 0;
    u32 stencilMask = std::numeric_limits<u32>::max();
    u32 stencilWriteMask = std::numeric_limits<u32>::max();
    ComparisonFunction depthBufferFunction = ComparisonFunction::LessEqual;
    bool depthBufferEnable = false;
    bool depthBufferWriteEnable = false;
    bool stencilEnable = false;

    [[nodiscard]] static DepthStencilDesc
    createDefault() noexcept;

    [[nodiscard]] static DepthStencilDesc
    createReadWriteDepth() noexcept;

    [[nodiscard]] static DepthStencilDesc
    createReadOnlyDepth() noexcept;

    [[nodiscard]] static DepthStencilDesc
    createNone() noexcept;
};

} // namespace pomdog::gpu
