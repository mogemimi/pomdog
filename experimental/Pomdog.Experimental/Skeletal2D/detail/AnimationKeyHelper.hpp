// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONKEYHELPER_1AC67582_HPP
#define POMDOG_ANIMATIONKEYHELPER_1AC67582_HPP

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

namespace AnimationKeyHelper {
    template <class Keyframe>
    bool Less(Keyframe const& a, Keyframe const& b)
    {
        return a.Time < b.Time;
    }
}

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_ANIMATIONKEYHELPER_1AC67582_HPP
