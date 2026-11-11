// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_blend_input.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class AnimationGraphState final {
public:
    /// key is the xxHash32 hash of the state name, as computeStringHash32()
    /// computes it. Animator::play() and Animator::crossFade() take it.
    u32 key;

    std::unique_ptr<AnimationNode> tree;
};

class AnimationGraph final {
public:
    /// states lists the states of the graph. The first state is the initial
    /// state, and no two states have the same key.
    std::vector<AnimationGraphState> states;

    /// inputs lists the parameters of the graph. No two parameters have the
    /// same key.
    std::vector<AnimationBlendInput> inputs;

    /// Returns the index of the parameter with the given xxHash32 key, or
    /// nullopt when the graph has no such parameter.
    std::optional<u16> findParameter(u32 key) const
    {
        // NOTE: A graph has a few parameters, so a linear search is enough.
        auto iter = std::find_if(std::begin(inputs), std::end(inputs), [&key](const AnimationBlendInput& input) {
            return input.key == key;
        });

        if (iter != std::end(inputs)) {
            POMDOG_ASSERT(inputs.size() <= std::numeric_limits<u16>::max());
            auto d = std::distance(std::begin(inputs), iter);

            POMDOG_ASSERT(d <= std::numeric_limits<u16>::max());
            return static_cast<u16>(d);
        }
        return std::nullopt;
    }
};

} // namespace pomdog::skeletal2d
