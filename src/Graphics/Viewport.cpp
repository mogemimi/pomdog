// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
Viewport::Viewport(Rectangle const& boundsIn) noexcept
    : Bounds(boundsIn)
    , MinDepth(0.0f)
    , MaxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(std::int32_t xIn, std::int32_t yIn,
    std::int32_t widthIn, std::int32_t heightIn) noexcept
    : Bounds(xIn, yIn, widthIn, heightIn)
    , MinDepth(0.0f)
    , MaxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(std::int32_t x, std::int32_t y,
    std::int32_t width, std::int32_t height,
    float minDepthIn, float maxDepthIn) noexcept
    : Bounds(x, y, width, height)
    , MinDepth(minDepthIn)
    , MaxDepth(maxDepthIn)
{}
//-----------------------------------------------------------------------
std::int32_t Viewport::GetX() const noexcept
{
    POMDOG_ASSERT(Bounds.X == Bounds.GetLeft());
    return Bounds.X;
}
//-----------------------------------------------------------------------
void Viewport::SetX(std::int32_t xIn) noexcept
{
    Bounds.X = xIn;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::GetY() const noexcept
{
    POMDOG_ASSERT(Bounds.Y == Bounds.GetTop());
    return Bounds.Y;
}
//-----------------------------------------------------------------------
void Viewport::SetY(std::int32_t yIn) noexcept
{
    Bounds.Y = yIn;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::GetWidth() const noexcept
{
    return Bounds.Width;
}
//-----------------------------------------------------------------------
void Viewport::SetWidth(std::int32_t widthIn) noexcept
{
    Bounds.Width = widthIn;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::GetHeight() const noexcept
{
    return Bounds.Height;
}
//-----------------------------------------------------------------------
void Viewport::SetHeight(std::int32_t heightIn) noexcept
{
    Bounds.Height = heightIn;
}
//-----------------------------------------------------------------------
float Viewport::GetAspectRatio() const
{
    POMDOG_ASSERT(Bounds.Height != 0);
    if (Bounds.Height != 0) {
        return static_cast<float>(Bounds.Width) / Bounds.Height;
    }
    return 0.0f;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
