// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT SamplerState {
public:
    SamplerState() noexcept;
    SamplerState(const SamplerState&) = delete;
    SamplerState& operator=(const SamplerState&) = delete;

    virtual ~SamplerState();
};

} // namespace Pomdog
