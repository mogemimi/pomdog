// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLSampler.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

class SamplerStateMetal final : public NativeSamplerState {
public:
    SamplerStateMetal(
        id<MTLDevice> device,
        SamplerDescription const& description);

    id<MTLSamplerState> GetSamplerState() const;

private:
    id<MTLSamplerState> samplerState;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
