// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::skeletal2d::detail::AnimationKeyHelper {

template <class Keyframe>
[[nodiscard]] bool Less(const Keyframe& a, const Keyframe& b) noexcept
{
    return a.Time < b.Time;
}

} // namespace pomdog::skeletal2d::detail::AnimationKeyHelper
