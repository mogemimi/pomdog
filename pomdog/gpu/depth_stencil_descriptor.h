// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/depth_stencil_operation.h"

namespace pomdog::gpu {

struct POMDOG_EXPORT DepthStencilDescriptor final {
    DepthStencilOperation clockwiseFace;
    DepthStencilOperation counterClockwiseFace;
    i32 referenceStencil;
    u32 stencilMask;
    u32 stencilWriteMask;
    ComparisonFunction depthBufferFunction;
    bool depthBufferEnable;
    bool depthBufferWriteEnable;
    bool stencilEnable;

    [[nodiscard]] static DepthStencilDescriptor
    createDefault() noexcept;

    [[nodiscard]] static DepthStencilDescriptor
    createReadWriteDepth() noexcept;

    [[nodiscard]] static DepthStencilDescriptor
    createReadOnlyDepth() noexcept;

    [[nodiscard]] static DepthStencilDescriptor
    createNone() noexcept;
};

} // namespace pomdog::gpu
