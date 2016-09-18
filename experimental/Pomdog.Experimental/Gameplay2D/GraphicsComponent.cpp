// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay2D/GraphicsComponent.hpp"

namespace Pomdog {

float GraphicsComponent::GetDrawOrder() const noexcept
{
    return drawOrder;
}

void GraphicsComponent::SetDrawOrder(float drawOrderIn) noexcept
{
    drawOrder = drawOrderIn;
}

bool GraphicsComponent::IsVisible() const noexcept
{
    return isVisible;
}

void GraphicsComponent::SetVisible(bool isVisibleIn) noexcept
{
    isVisible = isVisibleIn;
}

std::uint8_t ComponentTypeDeclaration<GraphicsComponent>::GetTypeIndex()
{
    return Detail::Gameplay::ComponentTypeIndex::Index<GraphicsComponent>();
}

} // namespace Pomdog
