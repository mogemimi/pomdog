// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_blend_input_type.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

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

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    std::vector<AnimationBlendInputType> types;
#endif
};

} // namespace pomdog::skeletal2d::detail
