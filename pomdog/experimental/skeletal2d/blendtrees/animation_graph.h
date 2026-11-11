// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_blend_input.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class AnimationGraphState final {
public:
    /// Key is the xxHash32 hash of the state name, as computeStringHash32()
    /// computes it. Animator::Play() and Animator::CrossFade() take it.
    u32 Key;

    std::unique_ptr<AnimationNode> Tree;
};

class AnimationGraph final {
public:
    /// States lists the states of the graph. The first state is the initial
    /// state, and no two states have the same key.
    std::vector<AnimationGraphState> States;

    /// Inputs lists the parameters of the graph. No two parameters have the
    /// same key.
    std::vector<AnimationBlendInput> Inputs;

    /// Returns the index of the parameter with the given xxHash32 key, or
    /// nullopt when the graph has no such parameter.
    std::optional<std::uint16_t> FindParameter(u32 key) const
    {
        // NOTE: A graph has a few parameters, so a linear search is enough.
        auto iter = std::find_if(std::begin(Inputs), std::end(Inputs), [&key](const AnimationBlendInput& input) {
            return input.Key == key;
        });

        if (iter != std::end(Inputs)) {
            POMDOG_ASSERT(Inputs.size() <= std::numeric_limits<std::uint16_t>::max());
            auto d = std::distance(std::begin(Inputs), iter);

            POMDOG_ASSERT(d <= std::numeric_limits<std::uint16_t>::max());
            return static_cast<std::uint16_t>(d);
        }
        return std::nullopt;
    }
};

} // namespace pomdog::skeletal2d
