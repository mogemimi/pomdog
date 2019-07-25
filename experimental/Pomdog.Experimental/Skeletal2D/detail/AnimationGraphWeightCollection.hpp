// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationGraphWeight.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class AnimationGraphWeightCollection {
public:
    void SetValue(std::uint16_t index, float value);
    void SetValue(std::uint16_t index, bool value);

    AnimationGraphWeight const& At(std::uint16_t index) const;

    std::uint16_t AddFloat();
    std::uint16_t AddBool();

    std::uint16_t Count() const;

    void Reserve(std::uint16_t size);

    void Reserve(std::size_t size);

private:
    std::vector<AnimationGraphWeight> parameters;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
