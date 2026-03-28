// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/post_process_compositor.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_buffer.h"
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
    Vector2 renderTargetSize;

    // {xy} = {rcpFrame.x, rcpFrame.y}
    Vector2 rcpFrame;
};

} // namespace

std::unique_ptr<Error>
PostProcessCompositor::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    if (auto err = screenQuad_.initialize(graphicsDevice); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize screen quad");
    }

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    POMDOG_ASSERT(presentationParameters.backBufferWidth > 0);
    POMDOG_ASSERT(presentationParameters.backBufferHeight > 0);

    viewport_.x = 0;
    viewport_.y = 0;
    viewport_.width = presentationParameters.backBufferWidth;
    viewport_.height = presentationParameters.backBufferHeight;

    if (auto [cb, cbErr] = graphicsDevice->createConstantBuffer(
            sizeof(PostProcessInfo),
            gpu::BufferUsage::Dynamic);
        cbErr != nullptr) {
        return errors::wrap(std::move(cbErr), "failed to create constant buffer for post process");
    }
    else {
        constantBuffer_ = std::move(cb);
    }

    if (auto err = buildRenderTargets(
            *graphicsDevice,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.backBufferFormat,
            presentationParameters.depthStencilFormat);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to build render targets");
    }

    return nullptr;
}

std::unique_ptr<Error>
PostProcessCompositor::setViewportSize(
    gpu::GraphicsDevice& graphicsDevice,
    int width,
    int height,
    gpu::PixelFormat depthFormat)
{
    POMDOG_ASSERT(!renderTargets_.empty());
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    if (viewport_.width == width && viewport_.height == height) {
        return nullptr;
    }

    viewport_.width = width;
    viewport_.height = height;

    if (auto err = buildRenderTargets(
            graphicsDevice,
            viewport_.width,
            viewport_.height,
            renderTargets_.front()->getFormat(),
            depthFormat);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to rebuild render targets");
    }

    return nullptr;
}

std::unique_ptr<Error>
PostProcessCompositor::buildRenderTargets(
    gpu::GraphicsDevice& graphicsDevice,
    int width,
    int height,
    gpu::PixelFormat surfaceFormat,
    gpu::PixelFormat depthFormat)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    for (auto& renderTarget : renderTargets_) {
        if (auto [rt, rtErr] = graphicsDevice.createRenderTarget2D(
                width,
                height,
                false,
                surfaceFormat);
            rtErr != nullptr) {
            return errors::wrap(std::move(rtErr), "failed to create render target");
        }
        else {
            renderTarget = std::move(rt);
        }
    }

    if (auto [ds, dsErr] = graphicsDevice.createDepthStencilBuffer(
            width,
            height,
            depthFormat);
        dsErr != nullptr) {
        return errors::wrap(std::move(dsErr), "failed to create depth stencil buffer");
    }
    else {
        depthStencilBuffer_ = std::move(ds);
    }

    return nullptr;
}

void PostProcessCompositor::updateConstantBuffer()
{
    POMDOG_ASSERT(viewport_.width > 0);
    POMDOG_ASSERT(viewport_.height > 0);

    PostProcessInfo info;
    info.renderTargetSize.x = static_cast<float>(viewport_.width);
    info.renderTargetSize.y = static_cast<float>(viewport_.height);
    info.rcpFrame = Vector2{1.0f / viewport_.width, 1.0f / viewport_.height};

    POMDOG_ASSERT(constantBuffer_);
    constantBuffer_->setData(0, gpu::makeByteSpan(info));
}

void PostProcessCompositor::composite(std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffectsIn)
{
    POMDOG_ASSERT(!imageEffectsIn.empty());
    imageEffects_ = std::move(imageEffectsIn);

    preRenderables_.clear();
    for (const auto& e : imageEffects_) {
        auto effect = std::dynamic_pointer_cast<ImageEffectPreRenderable>(e);
        if (effect) {
            preRenderables_.push_back(std::move(effect));
        }
    }
}

void PostProcessCompositor::composite(
    std::vector<std::shared_ptr<ImageEffectBase>>&& imageEffectsIn,
    std::vector<std::shared_ptr<ImageEffectPreRenderable>>&& preRenderableEffectsIn)
{
    POMDOG_ASSERT(!imageEffectsIn.empty());
    imageEffects_ = std::move(imageEffectsIn);

    preRenderables_ = std::move(preRenderableEffectsIn);
    for (const auto& e : imageEffects_) {
        auto effect = std::dynamic_pointer_cast<ImageEffectPreRenderable>(e);
        if (effect) {
            preRenderables_.push_back(std::move(effect));
        }
    }
    std::stable_sort(std::begin(preRenderables_), std::end(preRenderables_));
    preRenderables_.erase(
        std::unique(std::begin(preRenderables_), std::end(preRenderables_)),
        std::end(preRenderables_));
}

void PostProcessCompositor::draw(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source)
{
    if (imageEffects_.empty()) {
        return;
    }

    updateConstantBuffer();

    for (auto& effect : imageEffects_) {
        effect->updateGPUResources();
    }

    POMDOG_ASSERT(constantBuffer_);
    POMDOG_ASSERT(!renderTargets_.empty());
    POMDOG_ASSERT(renderTargets_.front());
    POMDOG_ASSERT(renderTargets_.back());

    auto readTarget = renderTargets_.front();
    auto writeTarget = renderTargets_.back();

    for (auto& effect : preRenderables_) {
        effect->preRender(commandList, constantBuffer_, [&] {
            screenQuad_.drawQuad(commandList);
        });
    }

    for (std::size_t index = 0; index < imageEffects_.size(); ++index) {
        std::shared_ptr<gpu::RenderTarget2D> currentSource;
        if (index == 0) {
            currentSource = source;
        }
        else {
            currentSource = readTarget;
        }

        auto& effect = imageEffects_[index];

        bool isLast = (index + 1) >= imageEffects_.size();
        if (isLast) {
            gpu::RenderPass renderPass;
            renderPass.renderTargets[0] = {nullptr, std::nullopt};
            renderPass.depthStencilBuffer = nullptr;
            renderPass.viewport = gpu::Viewport{viewport_};
            renderPass.scissorRect = viewport_;
            commandList.beginRenderPass(std::move(renderPass));
        }
        else {
            POMDOG_ASSERT(currentSource != writeTarget);
            gpu::RenderPass renderPass;
            renderPass.renderTargets[0] = {writeTarget, std::nullopt};
            renderPass.depthStencilBuffer = depthStencilBuffer_;
            renderPass.viewport = gpu::Viewport{viewport_};
            renderPass.scissorRect = viewport_;
            commandList.beginRenderPass(std::move(renderPass));
        }

        effect->apply(commandList, currentSource, constantBuffer_);

        screenQuad_.drawQuad(commandList);
        commandList.endRenderPass();
        std::swap(readTarget, writeTarget);
    }

    // Unbind texture
    commandList.setTexture(0);
}

bool PostProcessCompositor::canSkipPostProcess() const noexcept
{
    return imageEffects_.empty();
}

} // namespace pomdog
