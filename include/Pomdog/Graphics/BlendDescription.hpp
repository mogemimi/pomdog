// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/RenderTargetBlendDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <array>
#include <utility>

namespace Pomdog {

struct POMDOG_EXPORT BlendDescription {
    std::array<RenderTargetBlendDescription, 8> RenderTargets;
    bool AlphaToCoverageEnable;
    bool IndependentBlendEnable;

    static BlendDescription CreateDefault()
    {
        return CreateOpaque();
    }

    static BlendDescription CreateAdditive()
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto & renderTarget: desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateAdditive();
        }
        return std::move(desc);
    }

    static BlendDescription CreateAlphaBlend()
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto & renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateAlphaBlend();
        }
        return std::move(desc);
    }

    static BlendDescription CreateNonPremultiplied()
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto & renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateNonPremultiplied();
        }
        return std::move(desc);
    }

    static BlendDescription CreateOpaque()
    {
        BlendDescription desc;
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto & renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateOpaque();
        }
        return std::move(desc);
    }
};

} // namespace Pomdog
