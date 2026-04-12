// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class MouseImpl;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class MouseCocoa final {
private:
    std::shared_ptr<MouseImpl> impl_;
    f64 scrollWheel_ = 0.0;

public:
    explicit MouseCocoa(const std::shared_ptr<MouseImpl>& impl);

    void handleEvent(const SystemEvent& event);
};

} // namespace pomdog::detail::cocoa
