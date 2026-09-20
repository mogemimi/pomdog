// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class SystemEventQueue;
class MouseImpl;
} // namespace pomdog::detail

namespace pomdog::detail::emscripten {

class MouseEmscripten final {
private:
    std::shared_ptr<MouseImpl> impl_;

public:
    explicit MouseEmscripten(const std::shared_ptr<MouseImpl>& impl) noexcept;

    ~MouseEmscripten() noexcept;

    void subscribeEvent(const std::string& targetCanvas, const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    void unsubscribeEvent(const std::string& targetCanvas) noexcept;

    void handleMessage(const SystemEvent& event);
};

} // namespace pomdog::detail::emscripten
