// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/frame_counter.h"

namespace pomdog::gpu::detail::metal {

void FrameCounter::UpdateFrame() noexcept
{
    frameIndex_ = (frameIndex_ + 1) % GetMaxCount();
}

[[nodiscard]] std::uint32_t
FrameCounter::GetCurrentIndex() const noexcept
{
    return frameIndex_;
}

} // namespace pomdog::gpu::detail::metal
