// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationBlendInput.hpp"
#include "AnimationNode.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include <algorithm>

namespace Pomdog {

class AnimationGraphState {
public:
    std::string Name;
    std::unique_ptr<AnimationNode> Tree;
};

class AnimationGraph {
public:
    std::vector<AnimationGraphState> States;
    std::vector<AnimationBlendInput> Inputs;

    std::optional<std::uint16_t> FindParameter(const std::string& name) const
    {
        auto iter = std::find_if(std::begin(Inputs), std::end(Inputs), [&name](AnimationBlendInput const& input) {
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

} // namespace Pomdog
