// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/Texture2DView.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

std::int32_t Texture2DVariant::GetWidth() const
{
    POMDOG_ASSERT(variant != nullptr);
    if (index == Texture2DViewIndex::Texture2D) {
        auto texture = std::static_pointer_cast<Texture2D>(variant);
        POMDOG_ASSERT(texture != nullptr);
        return texture->GetWidth();
    }
    auto renderTarget = std::static_pointer_cast<RenderTarget2D>(variant);
    POMDOG_ASSERT(renderTarget != nullptr);
    return renderTarget->GetWidth();
}

std::int32_t Texture2DVariant::GetHeight() const
{
    POMDOG_ASSERT(variant != nullptr);
    if (index == Texture2DViewIndex::Texture2D) {
        auto texture = std::static_pointer_cast<Texture2D>(variant);
        POMDOG_ASSERT(texture != nullptr);
        return texture->GetHeight();
    }
    auto renderTarget = std::static_pointer_cast<RenderTarget2D>(variant);
    POMDOG_ASSERT(renderTarget != nullptr);
    return renderTarget->GetHeight();
}

SurfaceFormat Texture2DVariant::GetFormat() const
{
    POMDOG_ASSERT(variant != nullptr);
    if (index == Texture2DViewIndex::Texture2D) {
        auto texture = std::static_pointer_cast<Texture2D>(variant);
        POMDOG_ASSERT(texture != nullptr);
        return texture->GetFormat();
    }
    auto renderTarget = std::static_pointer_cast<RenderTarget2D>(variant);
    POMDOG_ASSERT(renderTarget != nullptr);
    return renderTarget->GetFormat();
}

Texture2DView::Texture2DView(std::nullptr_t)
{
}

Texture2DView::Texture2DView(const std::shared_ptr<Texture2D>& textureIn)
{
    body.variant = textureIn;
    body.index = Texture2DViewIndex::Texture2D;
}

Texture2DView::Texture2DView(std::shared_ptr<Texture2D>&& textureIn)
{
    body.variant = std::move(textureIn);
    body.index = Texture2DViewIndex::Texture2D;
}

Texture2DView::Texture2DView(std::shared_ptr<RenderTarget2D>&& renderTargetIn)
{
    body.variant = renderTargetIn;
    body.index = Texture2DViewIndex::RenderTarget2D;
}

Texture2DView::Texture2DView(const std::shared_ptr<RenderTarget2D>& renderTargetIn)
{
    body.variant = std::move(renderTargetIn);
    body.index = Texture2DViewIndex::RenderTarget2D;
}

Texture2DView& Texture2DView::operator=(const std::shared_ptr<Texture2D>& textureIn)
{
    body.variant = textureIn;
    body.index = Texture2DViewIndex::Texture2D;
    return *this;
}

Texture2DView& Texture2DView::operator=(std::shared_ptr<Texture2D>&& textureIn)
{
    body.variant = std::move(textureIn);
    body.index = Texture2DViewIndex::Texture2D;
    return *this;
}

Texture2DView& Texture2DView::operator=(std::shared_ptr<RenderTarget2D>&& renderTargetIn)
{
    body.variant = std::move(renderTargetIn);
    body.index = Texture2DViewIndex::RenderTarget2D;
    return *this;
}

Texture2DView& Texture2DView::operator=(const std::shared_ptr<RenderTarget2D>& renderTargetIn)
{
    body.variant = renderTargetIn;
    body.index = Texture2DViewIndex::RenderTarget2D;
    return *this;
}

Texture2DView& Texture2DView::operator=(std::nullptr_t)
{
    body.variant = nullptr;
    return *this;
}

bool Texture2DView::operator==(const Texture2DView& v) const noexcept
{
    return (body.index == v.body.index) && (body.variant == v.body.variant);
}

bool Texture2DView::operator==(std::nullptr_t) const noexcept
{
    return (body.variant == nullptr);
}

bool Texture2DView::operator!=(const Texture2DView& v) const noexcept
{
    return (body.index != v.body.index) || (body.variant != v.body.variant);
}

bool Texture2DView::operator!=(std::nullptr_t) const noexcept
{
    return (body.variant != nullptr);
}

const Texture2DVariant* Texture2DView::operator->() const noexcept
{
    return &body;
}

Texture2DView::operator bool() const noexcept
{
    return body.variant != nullptr;
}

Texture2DViewIndex Texture2DView::GetIndex() const noexcept
{
    return body.index;
}

std::shared_ptr<Texture2D> Texture2DView::AsTexture2D() const noexcept
{
    POMDOG_ASSERT(body.index == Texture2DViewIndex::Texture2D);
    return std::static_pointer_cast<Texture2D>(body.variant);
}

std::shared_ptr<RenderTarget2D> Texture2DView::AsRenderTarget2D() const noexcept
{
    POMDOG_ASSERT(body.index == Texture2DViewIndex::RenderTarget2D);
    return std::static_pointer_cast<RenderTarget2D>(body.variant);
}

} // namespace Pomdog
