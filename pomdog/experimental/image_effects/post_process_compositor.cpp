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
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    POMDOG_ASSERT(presentationParameters.backBufferWidth > 0);
    POMDOG_ASSERT(presentationParameters.backBufferHeight > 0);

    viewport.x = 0;
    viewport.y = 0;
    viewport.width = presentationParameters.backBufferWidth;
    viewport.height = presentationParameters.backBufferHeight;

    constantBuffer = std::get<0>(graphicsDevice->createConstantBuffer(
        sizeof(PostProcessInfo),
        gpu::BufferUsage::Dynamic));

    BuildRenderTargets(
        *graphicsDevice,
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        presentationParameters.backBufferFormat,
        presentationParameters.depthStencilFormat);
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

    if (viewport.width == width && viewport.height == height) {
        return;
    }

    viewport.width = width;
    viewport.height = height;

    BuildRenderTargets(
        graphicsDevice,
        viewport.width,
        viewport.height,
        renderTargets.front()->getFormat(),
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
        renderTarget = std::get<0>(graphicsDevice.createRenderTarget2D(
            width,
            height,
            false,
            surfaceFormat));
    }

    // FIXME: Add error handling
    depthStencilBuffer = std::get<0>(graphicsDevice.createDepthStencilBuffer(
        width,
        height,
        depthFormat));
}

void PostProcessCompositor::UpdateConstantBuffer()
{
    POMDOG_ASSERT(viewport.width > 0);
    POMDOG_ASSERT(viewport.height > 0);

    PostProcessInfo info;
    info.RenderTargetSize.x = static_cast<float>(viewport.width);
    info.RenderTargetSize.y = static_cast<float>(viewport.height);
    info.RcpFrame = Vector2{1.0f / viewport.width, 1.0f / viewport.height};

    POMDOG_ASSERT(constantBuffer);
    constantBuffer->setData(0, gpu::makeByteSpan(info));
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
            renderPass.renderTargets[0] = {nullptr, std::nullopt};
            renderPass.depthStencilBuffer = nullptr;
            renderPass.viewport = gpu::Viewport{viewport};
            renderPass.scissorRect = viewport;
            commandList.setRenderPass(std::move(renderPass));
        }
        else {
            POMDOG_ASSERT(currentSource != writeTarget);
            gpu::RenderPass renderPass;
            renderPass.renderTargets[0] = {writeTarget, std::nullopt};
            renderPass.depthStencilBuffer = depthStencilBuffer;
            renderPass.viewport = gpu::Viewport{viewport};
            renderPass.scissorRect = viewport;
            commandList.setRenderPass(std::move(renderPass));
        }

        effect->Apply(commandList, currentSource, constantBuffer);

        screenQuad.DrawQuad(commandList);
        std::swap(readTarget, writeTarget);
    }

    // Unbind texture
    commandList.setTexture(0);
}

bool PostProcessCompositor::CanSkipPostProcess() const noexcept
{
    return imageEffects.empty();
}

} // namespace pomdog
