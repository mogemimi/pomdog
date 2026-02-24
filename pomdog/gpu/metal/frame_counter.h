// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu::detail::metal {

class FrameCounter final {
public:
    void updateFrame() noexcept;

    [[nodiscard]] constexpr u32
    getMaxCount() const noexcept
    {
        // NOTE: Pomdog's metal renderer uses triple buffering.
        return 3;
    }

    [[nodiscard]] u32
    getCurrentIndex() const noexcept;

private:
    u32 frameIndex_ = 0;
};

} // namespace pomdog::gpu::detail::metal
