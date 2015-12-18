// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_RENDERQUEUE_32005F81_HPP
#define POMDOG_RENDERQUEUE_32005F81_HPP

#include <vector>
#include <memory>
#include <functional>

namespace Pomdog {

class RenderCommand;

class RenderQueue {
public:
    void PushBack(std::reference_wrapper<RenderCommand> && command);

    void Sort();

    std::size_t Count() const;

    void Clear();

    void Enumerate(std::function<void(RenderCommand &)> const& callback) const;

private:
    std::vector<std::reference_wrapper<RenderCommand>> negativeCommands;
    std::vector<std::reference_wrapper<RenderCommand>> positiveCommands;
    std::vector<std::reference_wrapper<RenderCommand>> zeroCommands;
};

} // namespace Pomdog

#endif // POMDOG_RENDERQUEUE_32005F81_HPP
