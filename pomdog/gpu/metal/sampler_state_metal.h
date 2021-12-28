// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/sampler_state.h"
#include "pomdog/utility/errors.h"
#import <Metal/MTLSampler.h>

namespace pomdog::detail::metal {

class SamplerStateMetal final : public SamplerState {
public:
    ~SamplerStateMetal() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        const SamplerDescription& description) noexcept;

    /// Gets the pointer of the native sampler object.
    id<MTLSamplerState> GetSamplerState() const noexcept;

private:
    id<MTLSamplerState> samplerState = nullptr;
};

} // namespace pomdog::detail::metal
