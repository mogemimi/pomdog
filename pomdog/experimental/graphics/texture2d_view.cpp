// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/texture2d_view.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

std::int32_t Texture2DVariant::GetWidth() const
{
    POMDOG_ASSERT(variant != nullptr);
    if (index == Texture2DViewIndex::Texture2D) {
        auto texture = std::static_pointer_cast<gpu::Texture2D>(variant);
        POMDOG_ASSERT(texture != nullptr);
        return texture->getWidth();
    }
    auto renderTarget = std::static_pointer_cast<gpu::RenderTarget2D>(variant);
    POMDOG_ASSERT(renderTarget != nullptr);
    return renderTarget->getWidth();
}

std::int32_t Texture2DVariant::GetHeight() const
{
    POMDOG_ASSERT(variant != nullptr);
    if (index == Texture2DViewIndex::Texture2D) {
        auto texture = std::static_pointer_cast<gpu::Texture2D>(variant);
        POMDOG_ASSERT(texture != nullptr);
        return texture->getHeight();
    }
    auto renderTarget = std::static_pointer_cast<gpu::RenderTarget2D>(variant);
    POMDOG_ASSERT(renderTarget != nullptr);
    return renderTarget->getHeight();
}

PixelFormat Texture2DVariant::GetFormat() const
{
    POMDOG_ASSERT(variant != nullptr);
    if (index == Texture2DViewIndex::Texture2D) {
        auto texture = std::static_pointer_cast<gpu::Texture2D>(variant);
        POMDOG_ASSERT(texture != nullptr);
        return texture->getFormat();
    }
    auto renderTarget = std::static_pointer_cast<gpu::RenderTarget2D>(variant);
    POMDOG_ASSERT(renderTarget != nullptr);
    return renderTarget->getFormat();
}

Texture2DView::Texture2DView(std::nullptr_t)
{
}

Texture2DView::Texture2DView(const std::shared_ptr<gpu::Texture2D>& textureIn)
{
    body.variant = textureIn;
    body.index = Texture2DViewIndex::Texture2D;
}

Texture2DView::Texture2DView(std::shared_ptr<gpu::Texture2D>&& textureIn)
{
    body.variant = std::move(textureIn);
    body.index = Texture2DViewIndex::Texture2D;
}

Texture2DView::Texture2DView(std::shared_ptr<gpu::RenderTarget2D>&& renderTargetIn)
{
    body.variant = renderTargetIn;
    body.index = Texture2DViewIndex::RenderTarget2D;
}

Texture2DView::Texture2DView(const std::shared_ptr<gpu::RenderTarget2D>& renderTargetIn)
{
    body.variant = std::move(renderTargetIn);
    body.index = Texture2DViewIndex::RenderTarget2D;
}

Texture2DView& Texture2DView::operator=(const std::shared_ptr<gpu::Texture2D>& textureIn)
{
    body.variant = textureIn;
    body.index = Texture2DViewIndex::Texture2D;
    return *this;
}

Texture2DView& Texture2DView::operator=(std::shared_ptr<gpu::Texture2D>&& textureIn)
{
    body.variant = std::move(textureIn);
    body.index = Texture2DViewIndex::Texture2D;
    return *this;
}

Texture2DView& Texture2DView::operator=(std::shared_ptr<gpu::RenderTarget2D>&& renderTargetIn)
{
    body.variant = std::move(renderTargetIn);
    body.index = Texture2DViewIndex::RenderTarget2D;
    return *this;
}

Texture2DView& Texture2DView::operator=(const std::shared_ptr<gpu::RenderTarget2D>& renderTargetIn)
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

std::shared_ptr<gpu::Texture2D> Texture2DView::AsTexture2D() const noexcept
{
    POMDOG_ASSERT(body.index == Texture2DViewIndex::Texture2D);
    return std::static_pointer_cast<gpu::Texture2D>(body.variant);
}

std::shared_ptr<gpu::RenderTarget2D> Texture2DView::AsRenderTarget2D() const noexcept
{
    POMDOG_ASSERT(body.index == Texture2DViewIndex::RenderTarget2D);
    return std::static_pointer_cast<gpu::RenderTarget2D>(body.variant);
}

} // namespace pomdog
