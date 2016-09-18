// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>
#include <algorithm>

namespace Pomdog {
namespace {

bool CompareRenderCommands(const RenderCommand& a, const RenderCommand& b)
{
    return a.GetDrawOrder() < b.GetDrawOrder();
}

} // unnamed namespace

void RenderQueue::PushBack(std::reference_wrapper<RenderCommand> && command)
{
    if (command.get().GetDrawOrder() > 0.0f) {
        positiveCommands.push_back(std::move(command));
    }
    else if (command.get().GetDrawOrder() == 0.0f) {
        zeroCommands.push_back(std::move(command));
    }
    else {
        negativeCommands.push_back(std::move(command));
    }
}

void RenderQueue::Sort()
{
    std::sort(std::begin(positiveCommands), std::end(positiveCommands), CompareRenderCommands);
    std::sort(std::begin(negativeCommands), std::end(negativeCommands), CompareRenderCommands);
}

std::size_t RenderQueue::GetCount() const noexcept
{
    return positiveCommands.size()
        + zeroCommands.size()
        + negativeCommands.size();
}

void RenderQueue::Clear()
{
    positiveCommands.clear();
    zeroCommands.clear();
    negativeCommands.clear();
}

void RenderQueue::Enumerate(const std::function<void(RenderCommand&)>& callback) const
{
    POMDOG_ASSERT(callback);

    for (auto & command: negativeCommands) {
        callback(command);
    }
    for (auto & command: zeroCommands) {
        callback(command);
    }
    for (auto & command: positiveCommands) {
        callback(command);
    }
}

} // namespace Pomdog
