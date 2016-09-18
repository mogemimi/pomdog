// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <vector>
#include <memory>
#include <functional>

namespace Pomdog {

class RenderCommand;

class RenderQueue final {
public:
    void PushBack(std::reference_wrapper<RenderCommand> && command);

    void Sort();

    std::size_t GetCount() const noexcept;

    void Clear();

    void Enumerate(const std::function<void(RenderCommand &)>& callback) const;

private:
    std::vector<std::reference_wrapper<RenderCommand>> negativeCommands;
    std::vector<std::reference_wrapper<RenderCommand>> positiveCommands;
    std::vector<std::reference_wrapper<RenderCommand>> zeroCommands;
};

} // namespace Pomdog
