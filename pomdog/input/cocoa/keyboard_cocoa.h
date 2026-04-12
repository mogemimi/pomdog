// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class KeyboardImpl;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class KeyboardCocoa final {
private:
    std::shared_ptr<KeyboardImpl> impl_;

public:
    explicit KeyboardCocoa(const std::shared_ptr<KeyboardImpl>& impl);

    void handleEvent(const SystemEvent& event);
};

} // namespace pomdog::detail::cocoa
