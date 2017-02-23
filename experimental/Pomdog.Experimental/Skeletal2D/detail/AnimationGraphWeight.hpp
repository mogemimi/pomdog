// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Skeletal2D/AnimationBlendInputType.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class AnimationGraphWeight {
public:
    explicit AnimationGraphWeight(float value);

    explicit AnimationGraphWeight(bool value);

    void SetValue(float value);
    void SetValue(bool value);

    float GetFloat() const;
    bool GetBool() const;

private:
    float weight;
    AnimationBlendInputType type;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
