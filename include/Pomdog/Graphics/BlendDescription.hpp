// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/RenderTargetBlendDescription.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct POMDOG_EXPORT BlendDescription final {
    std::array<RenderTargetBlendDescription, 8> RenderTargets;
    bool AlphaToCoverageEnable;
    bool IndependentBlendEnable;

    static BlendDescription CreateDefault() noexcept
    {
        return CreateOpaque();
    }

    static BlendDescription CreateAdditive() noexcept
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto& renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateAdditive();
        }
        return desc;
    }

    static BlendDescription CreateAlphaBlend() noexcept
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto& renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateAlphaBlend();
        }
        return desc;
    }

    static BlendDescription CreateNonPremultiplied() noexcept
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto& renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateNonPremultiplied();
        }
        return desc;
    }

    static BlendDescription CreateOpaque() noexcept
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto& renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateOpaque();
        }
        return desc;
    }
};

} // namespace Pomdog
