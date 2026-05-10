// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu::detail::metal {

/// FrameCounter tracks the current in-flight frame index for triple-buffered Metal rendering.
class FrameCounter final {
private:
    u32 frameIndex_ = 0;

public:
    /// Advances the frame index by one, wrapping at getMaxCount().
    void updateFrame() noexcept;

    [[nodiscard]] constexpr u32
    getMaxCount() const noexcept
    {
        // NOTE: Pomdog's metal renderer uses triple buffering.
        return 3;
    }

    /// Returns the current in-flight frame index in the range [0, getMaxCount()).
    [[nodiscard]] u32
    getCurrentIndex() const noexcept;
};

} // namespace pomdog::gpu::detail::metal
