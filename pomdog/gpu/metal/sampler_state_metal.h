// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLSampler.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {

class SamplerStateMetal final : public SamplerState {
public:
    ~SamplerStateMetal() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        const SamplerDescriptor& descriptor) noexcept;

    /// Gets the pointer of the native sampler object.
    [[nodiscard]] id<MTLSamplerState>
    getSamplerState() const noexcept;

private:
    id<MTLSamplerState> samplerState = nullptr;
};

} // namespace pomdog::gpu::detail::metal
