// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationBlendInputType.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
