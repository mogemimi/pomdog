// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Renderer.hpp"
#include "RenderCommand.hpp"
#include "Pomdog.Experimental/Rendering/RenderCommandProcessor.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Pomdog {
namespace {

bool CompareRenderCommands(const RenderCommand& a, const RenderCommand& b)
{
    if (a.GetDrawOrder() == b.GetDrawOrder()) {
        return a.GetType() < b.GetType();
    }
    return a.GetDrawOrder() < b.GetDrawOrder();
}

} // unnamed namespace

// MARK: - Renderer::Impl

class Renderer::Impl {
public:
    explicit Impl(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    void AddProcessor(
        const std::type_index& index,
        std::unique_ptr<RenderCommandProcessor> && processor);

    void Reset();

    std::shared_ptr<GraphicsCommandList> Render();

    void SortCommands();

public:
    std::unordered_map<
        std::type_index, std::unique_ptr<RenderCommandProcessor>> processors;
    std::vector<std::reference_wrapper<RenderCommand>> commands;
    std::shared_ptr<GraphicsCommandList> commandList;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    int drawCallCount;
    bool needToSortCommandList;
};

Renderer::Impl::Impl(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : viewMatrix(Matrix4x4::Identity)
    , projectionMatrix(Matrix4x4::Identity)
    , drawCallCount(0)
{
    POMDOG_ASSERT(graphicsDevice);
    commandList = std::make_shared<GraphicsCommandList>(graphicsDevice);
}

void Renderer::Impl::Reset()
{
    commands.clear();
}

void Renderer::Impl::AddProcessor(
    const std::type_index& index,
    std::unique_ptr<RenderCommandProcessor> && processor)
{
    POMDOG_ASSERT(processor);
    processors.emplace(index, std::move(processor));
}

void Renderer::Impl::SortCommands()
{
    if (!needToSortCommandList) {
        return;
    }
    std::sort(std::begin(commands), std::end(commands), CompareRenderCommands);
    needToSortCommandList = false;
}

std::shared_ptr<GraphicsCommandList> Renderer::Impl::Render()
{
    POMDOG_ASSERT(commandList);
    commandList->Reset();

    drawCallCount = 0;
    const auto viewProjection = viewMatrix * projectionMatrix;

    for (auto & iter : processors) {
        auto & processor = iter.second;
        POMDOG_ASSERT(processor);
        processor->Begin(commandList, viewProjection);
    }

    auto prevIter = std::end(processors);

    SortCommands();

    for (auto & command : commands) {
        auto iter = processors.find(command.get().GetType());

        if (prevIter != iter) {
            if (prevIter != std::end(processors)) {
                auto & processor = prevIter->second;
                POMDOG_ASSERT(processor);
                processor->FlushBatch();

                POMDOG_ASSERT(processor->GetDrawCallCount() >= 0);
                drawCallCount += processor->GetDrawCallCount();
            }
            prevIter = iter;
        }

        POMDOG_ASSERT(prevIter == iter);

        if (iter == std::end(processors)) {
            // NOTE: If the command processor is not found, skipping rendering.
            continue;
        }

        POMDOG_ASSERT(iter != std::end(processors));
        POMDOG_ASSERT(iter->second);

        auto & processor = iter->second;

        processor->Draw(commandList, command);
    }

    if (std::end(processors) != prevIter) {
        POMDOG_ASSERT(prevIter->second);
        auto & processor = prevIter->second;
        processor->End();

        POMDOG_ASSERT(processor->GetDrawCallCount() >= 0);
        drawCallCount += processor->GetDrawCallCount();
    }
    return commandList;
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
    impl->commands.push_back(std::move(command));
    impl->needToSortCommandList = true;
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

void Renderer::Reset()
{
    POMDOG_ASSERT(impl);
    impl->Reset();
}

void Renderer::AddProcessor(std::unique_ptr<RenderCommandProcessor> && processor)
{
    POMDOG_ASSERT(impl);
    auto typeIndex = processor->GetCommandType();
    impl->AddProcessor(std::move(typeIndex), std::move(processor));
}

} // namespace Pomdog
