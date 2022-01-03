// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {

class FrameCounter final {
public:
    void UpdateFrame() noexcept;

    [[nodiscard]] constexpr std::uint32_t
    GetMaxCount() const noexcept
    {
        // NOTE: Pomdog's metal renderer uses triple buffering.
        return 3;
    }

    [[nodiscard]] std::uint32_t
    GetCurrentIndex() const noexcept;

private:
    std::uint32_t frameIndex_ = 0;
};

} // namespace pomdog::gpu::detail::metal
