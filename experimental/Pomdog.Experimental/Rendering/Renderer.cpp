// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Renderer.hpp"
#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <unordered_map>
#include <typeindex>

namespace Pomdog {

// MARK: - Renderer::Impl

class Renderer::Impl {
public:
    Impl();

    void AddProcessor(
        std::type_index const& index,
        std::unique_ptr<RenderCommandProcessor> && processor);

    void Render(GraphicsCommandQueue & commandQueue);

    void Clear();

public:
    std::unordered_map<
        std::type_index, std::unique_ptr<RenderCommandProcessor>> processors;
    RenderQueue renderQueue;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    int drawCallCount;
};

Renderer::Impl::Impl()
    : viewMatrix(Matrix4x4::Identity)
    , projectionMatrix(Matrix4x4::Identity)
    , drawCallCount(0)
{
}

void Renderer::Impl::AddProcessor(
    std::type_index const& index,
    std::unique_ptr<RenderCommandProcessor> && processor)
{
    POMDOG_ASSERT(processor);
    processors.emplace(index, std::move(processor));
}

void Renderer::Impl::Render(GraphicsCommandQueue & commandQueue)
{
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
        auto iter = processors.find(command.TypeIndex());

        if (prevIter != iter) {
            if (prevIter != std::end(processors)) {
                auto & processor = prevIter->second;
                POMDOG_ASSERT(processor);
                processor->End(commandQueue);

                POMDOG_ASSERT(processor->GetDrawCallCount() >= 0);
                drawCallCount += processor->GetDrawCallCount();
            }

            if (iter != std::end(processors)) {
                auto & processor = iter->second;
                POMDOG_ASSERT(processor);
                processor->Begin(commandQueue);
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

        processor->Draw(commandQueue, command);
    });

    if (std::end(processors) != prevIter) {
        POMDOG_ASSERT(prevIter->second);
        auto & processor = prevIter->second;
        processor->End(commandQueue);

        POMDOG_ASSERT(processor->GetDrawCallCount() >= 0);
        drawCallCount += processor->GetDrawCallCount();
    }
}

void Renderer::Impl::Clear()
{
    renderQueue.Clear();
}

// MARK: - Renderer

Renderer::Renderer()
    : impl(std::make_unique<Impl>())
{}

Renderer::~Renderer() = default;

void Renderer::Render(GraphicsCommandQueue & commandQueue)
{
    POMDOG_ASSERT(impl);
    impl->Render(commandQueue);
}

void Renderer::PushCommand(std::reference_wrapper<RenderCommand> && command)
{
    POMDOG_ASSERT(impl);
    impl->renderQueue.PushBack(std::move(command));
}

void Renderer::SetViewMatrix(Matrix4x4 const& viewMatrixIn)
{
    POMDOG_ASSERT(impl);
    impl->viewMatrix = viewMatrixIn;
}

void Renderer::SetProjectionMatrix(Matrix4x4 const& projectionMatrixIn)
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

void Renderer::AddProcessor(
    std::type_index const& index,
    std::unique_ptr<RenderCommandProcessor> && processor)
{
    POMDOG_ASSERT(impl);
    impl->AddProcessor(index, std::move(processor));
}

} // namespace Pomdog
