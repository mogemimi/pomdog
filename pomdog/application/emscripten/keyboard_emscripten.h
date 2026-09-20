// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

#include <memory>

namespace pomdog::detail {
class SystemEvent;
class SystemEventQueue;
class KeyboardImpl;
} // namespace pomdog::detail

namespace pomdog::detail::emscripten {

class KeyboardEmscripten final {
private:
    std::shared_ptr<KeyboardImpl> impl_;

public:
    explicit KeyboardEmscripten(const std::shared_ptr<KeyboardImpl>& impl) noexcept;

    ~KeyboardEmscripten() noexcept;

    void subscribeEvent(const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    void unsubscribeEvent() noexcept;

    void handleMessage(const SystemEvent& event);
};

} // namespace pomdog::detail::emscripten
