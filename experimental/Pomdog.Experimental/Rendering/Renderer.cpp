// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Renderer.hpp"
#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <unordered_map>
#include <typeindex>

namespace Pomdog {

// MARK: - Renderer::Impl

class Renderer::Impl {
public:
    explicit Impl(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void AddProcessor(
        const std::type_index& index,
        std::unique_ptr<RenderCommandProcessor> && processor);

    std::shared_ptr<GraphicsCommandList> Render();

    void Clear();

public:
    std::unordered_map<
        std::type_index, std::unique_ptr<RenderCommandProcessor>> processors;
    RenderQueue renderQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    int drawCallCount;
};

Renderer::Impl::Impl(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : viewMatrix(Matrix4x4::Identity)
    , projectionMatrix(Matrix4x4::Identity)
    , drawCallCount(0)
{
    POMDOG_ASSERT(graphicsDevice);
    commandList = std::make_shared<GraphicsCommandList>(graphicsDevice);
}

void Renderer::Impl::AddProcessor(
    const std::type_index& index,
    std::unique_ptr<RenderCommandProcessor> && processor)
{
    POMDOG_ASSERT(processor);
    processors.emplace(index, std::move(processor));
}

std::shared_ptr<GraphicsCommandList> Renderer::Impl::Render()
{
    POMDOG_ASSERT(commandList);
    commandList->Reset();

    drawCallCount = 0;

    for (auto & iter: processors) {
        auto & processor = iter.second;
        POMDOG_ASSERT(processor);
        processor->SetViewProjection(viewMatrix, projectionMatrix);
    }

    auto prevIter = std::end(processors);

    renderQueue.Sort();
    renderQueue.Enumerate([&](RenderCommand & command)
    {
        auto iter = processors.find(command.GetType());

        if (prevIter != iter) {
            if (prevIter != std::end(processors)) {
                auto & processor = prevIter->second;
                POMDOG_ASSERT(processor);
                processor->End(commandList);

                POMDOG_ASSERT(processor->GetDrawCallCount() >= 0);
                drawCallCount += processor->GetDrawCallCount();
            }

            if (iter != std::end(processors)) {
                auto & processor = iter->second;
                POMDOG_ASSERT(processor);
                processor->Begin(commandList);
            }

            prevIter = iter;
        }

        POMDOG_ASSERT(prevIter == iter);

        if (iter == std::end(processors)) {
            // warning
            return;
        }

        POMDOG_ASSERT(iter != std::end(processors));
        POMDOG_ASSERT(iter->second);

        auto & processor = iter->second;

        processor->Draw(commandList, command);
    });

    if (std::end(processors) != prevIter) {
        POMDOG_ASSERT(prevIter->second);
        auto & processor = prevIter->second;
        processor->End(commandList);

        POMDOG_ASSERT(processor->GetDrawCallCount() >= 0);
        drawCallCount += processor->GetDrawCallCount();
    }
    return commandList;
}

void Renderer::Impl::Clear()
{
    renderQueue.Clear();
}

// MARK: - Renderer

Renderer::Renderer(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : impl(std::make_unique<Impl>(graphicsDevice))
{}

Renderer::~Renderer() = default;

std::shared_ptr<GraphicsCommandList> Renderer::Render()
{
    POMDOG_ASSERT(impl);
    return impl->Render();
}

void Renderer::PushCommand(std::reference_wrapper<RenderCommand> && command)
{
    POMDOG_ASSERT(impl);
    impl->renderQueue.PushBack(std::move(command));
}

void Renderer::SetViewMatrix(const Matrix4x4& viewMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->viewMatrix = viewMatrixIn;
}

void Renderer::SetProjectionMatrix(const Matrix4x4& projectionMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->projectionMatrix = projectionMatrixIn;
}

int Renderer::GetDrawCallCount() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->drawCallCount;
}

void Renderer::Clear()
{
    POMDOG_ASSERT(impl);
    impl->Clear();
}

void Renderer::AddProcessor(std::unique_ptr<RenderCommandProcessor> && processor)
{
    POMDOG_ASSERT(impl);
    auto typeIndex = processor->GetCommandType();
    impl->AddProcessor(std::move(typeIndex), std::move(processor));
}

} // namespace Pomdog
