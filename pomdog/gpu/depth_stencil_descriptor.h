// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/depth_stencil_operation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

struct POMDOG_EXPORT DepthStencilDescriptor final {
    DepthStencilOperation clockwiseFace;
    DepthStencilOperation counterClockwiseFace;
    std::int32_t referenceStencil;
    std::uint32_t stencilMask;
    std::uint32_t stencilWriteMask;
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
