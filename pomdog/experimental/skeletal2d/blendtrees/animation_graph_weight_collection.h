// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_blend_input_type.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

class AnimationGraphWeightCollection final {
public:
    void setValue(u16 index, f32 value);
    void setValue(u16 index, bool value);

    f32 getFloatAtIndex(u16 index) const;
    bool getBoolAtIndex(u16 index) const;

    u16 addFloat();
    u16 addBool();

    u16 getCount() const noexcept;

    void reserve(std::size_t size);

private:
    union Weight {
        f32 floatValue;
        bool boolValue;
    };

    std::vector<Weight> parameters_;

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    std::vector<AnimationBlendInputType> types_;
#endif
};

} // namespace pomdog::skeletal2d::detail
