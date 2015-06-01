// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
Viewport::Viewport(Rectangle const& bounds) noexcept
    : TopLeftX(bounds.X)
    , TopLeftY(bounds.Y)
    , Width(bounds.Width)
    , Height(bounds.Height)
    , MinDepth(0.0f)
    , MaxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(int xIn, int yIn, int widthIn, int heightIn) noexcept
    : TopLeftX(xIn)
    , TopLeftY(yIn)
    , Width(widthIn)
    , Height(heightIn)
    , MinDepth(0.0f)
    , MaxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(int xIn, int yIn, int widthIn, int heightIn,
    float minDepthIn, float maxDepthIn) noexcept
    : TopLeftX(xIn)
    , TopLeftY(yIn)
    , Width(widthIn)
    , Height(heightIn)
    , MinDepth(minDepthIn)
    , MaxDepth(maxDepthIn)
{}
//-----------------------------------------------------------------------
Rectangle Viewport::GetBounds() const noexcept
{
    return Rectangle{TopLeftX, TopLeftY, Width, Height};
}
//-----------------------------------------------------------------------
float Viewport::GetAspectRatio() const
{
    POMDOG_ASSERT(this->Height != 0);
    if (this->Height != 0) {
        return static_cast<float>(this->Width) / this->Height;
    }
    return 0.0f;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
