// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_blend_input.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class AnimationGraphState final {
public:
    std::string Name;
    std::unique_ptr<AnimationNode> Tree;
};

class AnimationGraph final {
public:
    std::vector<AnimationGraphState> States;
    std::vector<AnimationBlendInput> Inputs;

    std::optional<std::uint16_t> FindParameter(const std::string& name) const
    {
        auto iter = std::find_if(std::begin(Inputs), std::end(Inputs), [&name](const AnimationBlendInput& input) {
            return input.Name == name;
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
