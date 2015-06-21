// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "PostProcessCompositor.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

struct PostProcessInfo {
    // {xy} = {width, height}
    Vector2 RenderTargetSize;

    // {xy} = {rcpFrame.x, rcpFrame.y}
    Vector2 RcpFrame;
};

} // unnamed namespace
//-----------------------------------------------------------------------
PostProcessCompositor::PostProcessCompositor(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    int width, int height, SurfaceFormat surfaceFormat)
    : screenQuad(graphicsDevice)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = width;
    viewport.Height = height;

    constantBuffer = std::make_shared<ConstantBuffer>(
        *graphicsDevice,
        sizeof(PostProcessInfo),
        BufferUsage::Dynamic);

    BuildRenderTargets(*graphicsDevice, width, height, surfaceFormat);
    UpdateConstantBuffer();
}
//-----------------------------------------------------------------------
void PostProcessCompositor::SetViewportSize(
    GraphicsDevice & graphicsDevice, int width, int height)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    if (viewport.Width == width && viewport.Height == height) {
        return;
    }

    viewport.Width = width;
    viewport.Height = height;

    BuildRenderTargets(graphicsDevice,
        viewport.Width, viewport.Height,
        renderTargets.front()->Format());
    UpdateConstantBuffer();
}
//-----------------------------------------------------------------------
void PostProcessCompositor::BuildRenderTargets(
    GraphicsDevice & graphicsDevice,
    int width, int height, SurfaceFormat surfaceFormat)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    for (auto & renderTarget: renderTargets) {
        renderTarget = std::make_shared<RenderTarget2D>(
            graphicsDevice,
            width, height, false, surfaceFormat, DepthFormat::None);
    }
}
//-----------------------------------------------------------------------
void PostProcessCompositor::UpdateConstantBuffer()
{
    POMDOG_ASSERT(viewport.Width > 0);
    POMDOG_ASSERT(viewport.Height > 0);

    PostProcessInfo info;
    info.RenderTargetSize.X = viewport.Width;
    info.RenderTargetSize.Y = viewport.Height;
    info.RcpFrame = Vector2{1.0f / viewport.Width, 1.0f / viewport.Height};

    POMDOG_ASSERT(constantBuffer);
    constantBuffer->SetValue(info);
}
//-----------------------------------------------------------------------
void PostProcessCompositor::Composite(std::vector<std::shared_ptr<ImageEffectBase>> && imageEffectsIn)
{
    POMDOG_ASSERT(!imageEffectsIn.empty());
    imageEffects = std::move(imageEffectsIn);

    for (auto & effect : imageEffects) {
        POMDOG_ASSERT(effect);
        effect->BindConstantBuffer(constantBuffer);
    }
}
//-----------------------------------------------------------------------
void PostProcessCompositor::Draw(
    GraphicsCommandList & commandList,
    std::shared_ptr<RenderTarget2D> const& source)
{
    if (imageEffects.empty()) {
        return;
    }

    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(renderTargets.front());
    POMDOG_ASSERT(renderTargets.back());

    commandList.SetViewport(Viewport{viewport});
    commandList.SetScissorRectangle(viewport);

    auto readTarget = renderTargets.front();
    auto writeTarget = renderTargets.back();

    for (std::size_t index = 0; index < imageEffects.size(); ++index) {
        std::shared_ptr<RenderTarget2D> currentSource;
        if (index == 0) {
            currentSource = source;
        } else {
            currentSource = readTarget;
        }

        bool isLast = (index + 1) >= imageEffects.size();
        if (isLast) {
            commandList.SetRenderTarget();
        } else {
            POMDOG_ASSERT(currentSource != writeTarget);
            commandList.SetRenderTarget(writeTarget);
        }

        auto & effect = imageEffects[index];
        effect->Apply(commandList, currentSource);

        screenQuad.DrawQuad(commandList);
        std::swap(readTarget, writeTarget);
    }

    // Unbind texture
    commandList.SetTexture(0);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
