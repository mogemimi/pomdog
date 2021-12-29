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
    DepthStencilOperation ClockwiseFace;
    DepthStencilOperation CounterClockwiseFace;
    std::int32_t ReferenceStencil;
    std::uint32_t StencilMask;
    std::uint32_t StencilWriteMask;
    ComparisonFunction DepthBufferFunction;
    bool DepthBufferEnable;
    bool DepthBufferWriteEnable;
    bool StencilEnable;

    [[nodiscard]] static DepthStencilDescriptor
    CreateDefault() noexcept;

    [[nodiscard]] static DepthStencilDescriptor
    CreateReadWriteDepth() noexcept;

    [[nodiscard]] static DepthStencilDescriptor
    CreateReadOnlyDepth() noexcept;

    [[nodiscard]] static DepthStencilDescriptor
    CreateNone() noexcept;
};

} // namespace pomdog::gpu
