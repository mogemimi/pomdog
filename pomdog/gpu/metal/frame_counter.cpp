// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/frame_counter.h"

namespace pomdog::gpu::detail::metal {

void FrameCounter::updateFrame() noexcept
{
    frameIndex_ = (frameIndex_ + 1) % getMaxCount();
}

[[nodiscard]] std::uint32_t
FrameCounter::getCurrentIndex() const noexcept
{
    return frameIndex_;
}

} // namespace pomdog::gpu::detail::metal
