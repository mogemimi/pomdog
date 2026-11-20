// Copyright mogemimi. Distributed under the MIT license.

#include "clear_color_test.h"
#include "pomdog/application/game_host.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/color.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

std::unique_ptr<Error>
ClearColorTest::initialize(const std::shared_ptr<GameHost>& gameHost)
{
    graphicsDevice_ = gameHost->getGraphicsDevice();
    commandQueue_ = gameHost->getCommandQueue();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    return nullptr;
}

void ClearColorTest::update()
{
}

void ClearColorTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();

    // NOTE: A render pass with a clear color clears the render target when the
    // pass begins. A null render target is the back buffer.
    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));
    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
