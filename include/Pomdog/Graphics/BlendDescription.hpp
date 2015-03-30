// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLENDDESCRIPTION_0C296679_HPP
#define POMDOG_BLENDDESCRIPTION_0C296679_HPP

#include "RenderTargetBlendDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <limits>
#include <array>
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT BlendDescription {
public:
    std::array<RenderTargetBlendDescription, 8> RenderTargets;
    std::uint32_t MultiSampleMask;
    bool AlphaToCoverageEnable;
    bool IndependentBlendEnable;

public:
    static BlendDescription CreateDefault()
    {
        return CreateOpaque();
    }

    static BlendDescription CreateAdditive()
    {
        BlendDescription desc;
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
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
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
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
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
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
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        for (auto & renderTarget : desc.RenderTargets) {
            renderTarget = RenderTargetBlendDescription::CreateOpaque();
        }
        return std::move(desc);
    }
};

} // namespace Pomdog

#endif // POMDOG_BLENDDESCRIPTION_0C296679_HPP
