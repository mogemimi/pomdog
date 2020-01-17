// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationBlendInputType.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Pomdog::Skeletal2D::Detail {

class AnimationGraphWeightCollection final {
public:
    void SetValue(std::uint16_t index, float value);
    void SetValue(std::uint16_t index, bool value);

    float GetFloatAtIndex(std::uint16_t index) const;
    bool GetBoolAtIndex(std::uint16_t index) const;

    std::uint16_t AddFloat();
    std::uint16_t AddBool();

    std::uint16_t GetCount() const noexcept;

    void Reserve(std::size_t size);

private:
    union Weight {
        float floatValue;
        bool boolValue;
    };

    std::vector<Weight> parameters;

#if defined(DEBUG) && !defined(NDEBUG)
    std::vector<AnimationBlendInputType> types;
#endif
};

} // namespace Pomdog::Skeletal2D::Detail
