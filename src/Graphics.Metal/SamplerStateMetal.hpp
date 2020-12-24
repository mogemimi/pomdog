// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Utility/Errors.hpp"
#import <Metal/MTLSampler.h>

namespace Pomdog::Detail::Metal {

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

} // namespace Pomdog::Detail::Metal
