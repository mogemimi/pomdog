// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/keys.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bitset>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class SystemEventQueue;
class KeyboardImpl;
} // namespace pomdog::detail

namespace pomdog::detail::emscripten {

class KeyboardEmscripten final {
private:
    std::shared_ptr<KeyboardImpl> impl_;

    // NOTE: A key that goes up in the frame it went down stays down for that
    // frame and goes up when the next frame begins, so that the game sees the
    // press. Software keyboards on phones send both events at once.
    std::bitset<MaxKeysCount> pressedInFrame_;
    std::bitset<MaxKeysCount> releasePending_;

public:
    explicit KeyboardEmscripten(const std::shared_ptr<KeyboardImpl>& impl) noexcept;

    ~KeyboardEmscripten() noexcept;

    void subscribeEvent(const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    void unsubscribeEvent() noexcept;

    /// Releases the keys that went up in the previous frame right after they
    /// went down. Call it at the beginning of each frame, before the events.
    void beginFrame() noexcept;

    void handleMessage(const SystemEvent& event);
};

} // namespace pomdog::detail::emscripten
