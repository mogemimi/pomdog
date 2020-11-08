// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#import <Metal/MTLSampler.h>

namespace Pomdog::Detail::Metal {

class SamplerStateMetal final : public NativeSamplerState {
public:
    SamplerStateMetal(
        id<MTLDevice> device,
        const SamplerDescription& description);

    id<MTLSamplerState> GetSamplerState() const noexcept;

private:
    id<MTLSamplerState> samplerState;
};

} // namespace Pomdog::Detail::Metal
