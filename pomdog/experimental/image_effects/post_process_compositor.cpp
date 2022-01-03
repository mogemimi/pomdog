// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/post_process_compositor.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

struct PostProcessInfo {
    // {xy} = {width, height}
    Vector2 RenderTargetSize;

    // {xy} = {rcpFrame.x, rcpFrame.y}
    Vector2 RcpFrame;
};

} // namespace

PostProcessCompositor::PostProcessCompositor(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
    : screenQuad(graphicsDevice)
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    POMDOG_ASSERT(presentationParameters.BackBufferWidth > 0);
    POMDOG_ASSERT(presentationParameters.BackBufferHeight > 0);

    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = presentationParameters.BackBufferWidth;
    viewport.Height = presentationParameters.BackBufferHeight;

    constantBuffer = std::get<0>(graphicsDevice->CreateConstantBuffer(
        sizeof(PostProcessInfo),
        gpu::BufferUsage::Dynamic));

    BuildRenderTargets(
        *graphicsDevice,
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        presentationParameters.BackBufferFormat,
        presentationParameters.DepthStencilFormat);
}

void PostProcessCompositor::SetViewportSize(
    gpu::GraphicsDevice& graphicsDevice,
    int width,
    int height,
    PixelFormat depthFormat)
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
}

void PostProcessCompositor::BuildRenderTargets(
    gpu::GraphicsDevice& graphicsDevice,
    int width,
    int height,
    PixelFormat surfaceFormat,
    PixelFormat depthFormat)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    for (auto& renderTarget : renderTargets) {
        // FIXME: Add error handling
        renderTarget = std::get<0>(graphicsDevice.CreateRenderTarget2D(
            width,
            height,
            false,
            surfaceFormat));
    }

    // FIXME: Add error handling
    depthStencilBuffer = std::get<0>(graphicsDevice.CreateDepthStencilBuffer(
        width,
        height,
        depthFormat));
}

void PostProcessCompositor::UpdateConstantBuffer()
{
    POMDOG_ASSERT(viewport.Width > 0);
    POMDOG_ASSERT(viewport.Height > 0);

    PostProcessInfo info;
    info.RenderTargetSize.X = static_cast<float>(viewport.Width);
    info.RenderTargetSize.Y = static_cast<float>(viewport.Height);
    info.RcpFrame = Vector2{1.0f / viewport.Width, 1.0f / viewport.Height};

    POMDOG_ASSERT(constantBuffer);
    constantBuffer->SetData(0, gpu::MakeByteSpan(info));
}

void PostProcessCompositor::Composite(std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffectsIn)
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
    std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffectsIn,
    std::vector<std::shared_ptr<ImageEffectPreRenderable>>&& preRenderableEffectsIn)
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
    std::stable_sort(std::begin(preRenderables), std::end(preRenderables));
    preRenderables.erase(
        std::unique(std::begin(preRenderables), std::end(preRenderables)),
        std::end(preRenderables));
}

void PostProcessCompositor::Draw(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source)
{
    if (imageEffects.empty()) {
        return;
    }

    UpdateConstantBuffer();

    for (auto& effect : imageEffects) {
        effect->UpdateGPUResources();
    }

    POMDOG_ASSERT(constantBuffer);
    POMDOG_ASSERT(!renderTargets.empty());
    POMDOG_ASSERT(renderTargets.front());
    POMDOG_ASSERT(renderTargets.back());

    auto readTarget = renderTargets.front();
    auto writeTarget = renderTargets.back();

    for (auto& effect : preRenderables) {
        effect->PreRender(commandList, constantBuffer, [&] {
            screenQuad.DrawQuad(commandList);
        });
    }

    for (std::size_t index = 0; index < imageEffects.size(); ++index) {
        std::shared_ptr<gpu::RenderTarget2D> currentSource;
        if (index == 0) {
            currentSource = source;
        }
        else {
            currentSource = readTarget;
        }

        auto& effect = imageEffects[index];

        bool isLast = (index + 1) >= imageEffects.size();
        if (isLast) {
            gpu::RenderPass renderPass;
            renderPass.RenderTargets[0] = {nullptr, std::nullopt};
            renderPass.DepthStencilBuffer = nullptr;
            renderPass.Viewport = gpu::Viewport{viewport};
            renderPass.ScissorRect = viewport;
            commandList.SetRenderPass(std::move(renderPass));
        }
        else {
            POMDOG_ASSERT(currentSource != writeTarget);
            gpu::RenderPass renderPass;
            renderPass.RenderTargets[0] = {writeTarget, std::nullopt};
            renderPass.DepthStencilBuffer = depthStencilBuffer;
            renderPass.Viewport = gpu::Viewport{viewport};
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

} // namespace pomdog
