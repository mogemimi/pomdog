// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT SamplerState final {
public:
    SamplerState() = delete;
    SamplerState(const SamplerState&) = delete;
    SamplerState & operator=(const SamplerState&) = delete;

    SamplerState(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const SamplerDescription& description);

    ~SamplerState();

    Detail::NativeSamplerState* GetNativeSamplerState();

private:
    std::unique_ptr<Detail::NativeSamplerState> nativeSamplerState;
};

} // namespace Pomdog
