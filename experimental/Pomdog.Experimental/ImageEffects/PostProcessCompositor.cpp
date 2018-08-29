// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "PostProcessCompositor.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstring>

namespace Pomdog {
namespace {

struct PostProcessInfo {
    // {xy} = {width, height}
    Vector2 RenderTargetSize;

    // {xy} = {rcpFrame.x, rcpFrame.y}
    Vector2 RcpFrame;
};

} // unnamed namespace

PostProcessCompositor::PostProcessCompositor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : screenQuad(graphicsDevice)
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    POMDOG_ASSERT(presentationParameters.BackBufferWidth > 0);
    POMDOG_ASSERT(presentationParameters.BackBufferHeight > 0);

    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = presentationParameters.BackBufferWidth;
    viewport.Height = presentationParameters.BackBufferHeight;

    constantBuffer = std::make_shared<ConstantBuffer>(
        *graphicsDevice,
        sizeof(PostProcessInfo),
        BufferUsage::Dynamic);

    BuildRenderTargets(
        *graphicsDevice,
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        presentationParameters.BackBufferFormat,
        presentationParameters.DepthStencilFormat);
    UpdateConstantBuffer();
}

void PostProcessCompositor::SetViewportSize(
    GraphicsDevice & graphicsDevice,
    int width,
    int height,
    DepthFormat depthFormat)
{
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    if (viewport.Width == width && viewport.Height == height) {
        return;
    }

    viewport.Width = width;
    viewport.Height = height;

    BuildRenderTargets(
        graphicsDevice,
        viewport.Width,
        viewport.Height,
        renderTargets.front()->GetFormat(),
        depthFormat);
    UpdateConstantBuffer();
}

void PostProcessCompositor::BuildRenderTargets(
    GraphicsDevice & graphicsDevice,
    int width,
    int height,
    SurfaceFormat surfaceFormat,
    DepthFormat depthFormat)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    for (auto & renderTarget: renderTargets) {
        renderTarget = std::make_shared<RenderTarget2D>(
            graphicsDevice,
            width,
            height,
            false,
            surfaceFormat,
            depthFormat);
    }
}

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

void PostProcessCompositor::Composite(std::vector<std::shared_ptr<ImageEffectBase>> && imageEffectsIn)
{
    POMDOG_ASSERT(!imageEffectsIn.empty());
    imageEffects = std::move(imageEffectsIn);

    preRenderables.clear();
    for (const auto& e : imageEffects) {
        auto effect = std::dynamic_pointer_cast<ImageEffectPreRenderable>(e);
        if (effect) {
            preRenderables.push_back(std::move(effect));
        }
    }
}

void PostProcessCompositor::Composite(
    std::vector<std::shared_ptr<ImageEffectBase>> && imageEffectsIn,
    std::vector<std::shared_ptr<ImageEffectPreRenderable>> && preRenderableEffectsIn)
{
    POMDOG_ASSERT(!imageEffectsIn.empty());
    imageEffects = std::move(imageEffectsIn);

    preRenderables = std::move(preRenderableEffectsIn);
    for (const auto& e : imageEffects) {
        auto effect = std::dynamic_pointer_cast<ImageEffectPreRenderable>(e);
        if (effect) {
            preRenderables.push_back(std::move(effect));
        }
    }
    std::sort(std::begin(preRenderables), std::end(preRenderables));
    preRenderables.erase(
        std::unique(std::begin(preRenderables), std::end(preRenderables)),
        std::end(preRenderables));
}

void PostProcessCompositor::Draw(
    GraphicsCommandList & commandList,
    std::shared_ptr<RenderTarget2D> const& source)
{
    if (imageEffects.empty()) {
        return;
    }

    POMDOG_ASSERT(constantBuffer);
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(renderTargets.front());
    POMDOG_ASSERT(renderTargets.back());

    auto readTarget = renderTargets.front();
    auto writeTarget = renderTargets.back();

    for (auto & effect : preRenderables) {
        effect->PreRender(commandList, constantBuffer, [&] {
            screenQuad.DrawQuad(commandList);
        });
    }

    for (std::size_t index = 0; index < imageEffects.size(); ++index) {
        std::shared_ptr<RenderTarget2D> currentSource;
        if (index == 0) {
            currentSource = source;
        } else {
            currentSource = readTarget;
        }

        auto & effect = imageEffects[index];

        bool isLast = (index + 1) >= imageEffects.size();
        if (isLast) {
            RenderPass renderPass;
            renderPass.RenderTargets.emplace_back(nullptr, std::nullopt);
            renderPass.Viewport = Viewport{viewport};
            renderPass.ScissorRect = viewport;
            commandList.SetRenderPass(std::move(renderPass));
        }
        else {
            POMDOG_ASSERT(currentSource != writeTarget);
            RenderPass renderPass;
            renderPass.RenderTargets.emplace_back(writeTarget, std::nullopt);
            renderPass.Viewport = Viewport{viewport};
            renderPass.ScissorRect = viewport;
            commandList.SetRenderPass(std::move(renderPass));
        }

        effect->Apply(commandList, currentSource, constantBuffer);

        screenQuad.DrawQuad(commandList);
        std::swap(readTarget, writeTarget);
    }

    // Unbind texture
    commandList.SetTexture(0);
}

bool PostProcessCompositor::CanSkipPostProcess() const noexcept
{
    return imageEffects.empty();
}

} // namespace Pomdog
