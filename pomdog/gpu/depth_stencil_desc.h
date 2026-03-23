// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/depth_stencil_operation.h"

namespace pomdog::gpu {

struct POMDOG_EXPORT DepthStencilDesc final {
    DepthStencilOperation clockwiseFace;
    DepthStencilOperation counterClockwiseFace;
    i32 referenceStencil;
    u32 stencilMask;
    u32 stencilWriteMask;
    ComparisonFunction depthBufferFunction;
    bool depthBufferEnable;
    bool depthBufferWriteEnable;
    bool stencilEnable;

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
