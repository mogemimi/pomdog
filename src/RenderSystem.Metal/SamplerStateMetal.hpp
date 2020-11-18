// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#import <Metal/MTLSampler.h>

namespace Pomdog::Detail::Metal {

class SamplerStateMetal final : public SamplerState {
public:
    SamplerStateMetal(
        id<MTLDevice> device,
        const SamplerDescription& description);

    id<MTLSamplerState> GetSamplerState() const noexcept;

private:
    id<MTLSamplerState> samplerState = nullptr;
};

} // namespace Pomdog::Detail::Metal
