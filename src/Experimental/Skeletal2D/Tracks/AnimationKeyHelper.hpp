// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Skeletal2D::Detail::AnimationKeyHelper {

template <class Keyframe>
[[nodiscard]] bool Less(const Keyframe& a, const Keyframe& b) noexcept
{
    return a.Time < b.Time;
}

} // namespace Pomdog::Skeletal2D::Detail::AnimationKeyHelper
