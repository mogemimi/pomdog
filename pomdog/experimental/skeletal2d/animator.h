// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class AnimationGraph;
class Skeleton;
class SkeletonPose;

/// Animator plays the states of an animation graph on a skeleton pose.
///
/// The methods take states and parameters by the xxHash32 hashes of their
/// names. Compute a key with computeStringHash32(), which hashes a string
/// literal at compile time:
/// ```cpp
/// animator->setFloat(computeStringHash32("Run.Weight"), 0.5f);
/// animator->play(computeStringHash32("Idle"));
/// ```
class Animator final {
public:
    Animator(
        const std::shared_ptr<Skeleton>& skeleton,
        const std::shared_ptr<skeletal2d::SkeletonPose>& skeletonPose,
        const std::shared_ptr<AnimationGraph>& animationGraph);

    ~Animator();

    void update(const Duration& frameDuration);

    /// Blends from the current state to the state with the given key over
    /// transitionDuration. Does nothing while another cross-fade is running.
    void crossFade(u32 stateKey, const Duration& transitionDuration);

    /// Plays the state with the given key from its beginning.
    void play(u32 stateKey);

    f32 getPlaybackRate() const noexcept;

    void setPlaybackRate(f32 playbackRate) noexcept;

    /// Sets the float parameter with the given key. Does nothing when the
    /// graph has no such parameter.
    void setFloat(u32 parameterKey, f32 value);

    /// Sets the bool parameter with the given key. Does nothing when the graph
    /// has no such parameter.
    void setBool(u32 parameterKey, bool value);

    /// Returns the key of the current state. During a cross-fade, it is the
    /// state that the cross-fade started from.
    [[nodiscard]] u32 getCurrentStateKey() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace pomdog::skeletal2d
