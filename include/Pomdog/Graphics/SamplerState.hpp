// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SAMPLERSTATE_87568CD5_HPP
#define POMDOG_SAMPLERSTATE_87568CD5_HPP

#include "detail/ForwardDeclarations.hpp"
#include "SamplerDescription.hpp"
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
    Detail::RenderSystem::NativeSamplerState* NativeSamplerState();

private:
    std::unique_ptr<Detail::RenderSystem::NativeSamplerState> nativeSamplerState;
};

} // namespace Pomdog

#endif // POMDOG_SAMPLERSTATE_87568CD5_HPP
