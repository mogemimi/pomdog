// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT SamplerState {
public:
    SamplerState() = delete;
    SamplerState(SamplerState const&) = delete;
    SamplerState & operator=(SamplerState const&) = delete;

    SamplerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        SamplerDescription const& description);

    ~SamplerState();

public:
    Detail::NativeSamplerState* NativeSamplerState();

private:
    std::unique_ptr<Detail::NativeSamplerState> nativeSamplerState;
};

} // namespace Pomdog
