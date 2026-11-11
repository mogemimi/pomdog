// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::skeletal2d::detail::AnimationKeyHelper {

template <class Keyframe>
[[nodiscard]] bool less(const Keyframe& a, const Keyframe& b) noexcept
{
    return a.time < b.time;
}

} // namespace pomdog::skeletal2d::detail::AnimationKeyHelper
