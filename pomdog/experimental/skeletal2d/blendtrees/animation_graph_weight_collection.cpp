// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

u16 AnimationGraphWeightCollection::addFloat()
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types_.size() == parameters_.size());
    types_.push_back(AnimationBlendInputType::Float);
#endif
    Weight weight;
    weight.floatValue = 0.0f;
    parameters_.push_back(std::move(weight));
    return static_cast<u16>(parameters_.size()) - 1;
}

u16 AnimationGraphWeightCollection::addBool()
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types_.size() == parameters_.size());
    types_.push_back(AnimationBlendInputType::Bool);
#endif
    Weight weight;
    weight.boolValue = false;
    parameters_.push_back(std::move(weight));
    return static_cast<u16>(parameters_.size()) - 1;
}

void AnimationGraphWeightCollection::setValue(u16 index, f32 value)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types_[index] == AnimationBlendInputType::Float);
#endif
    POMDOG_ASSERT(index < parameters_.size());
    parameters_[index].floatValue = value;
}

void AnimationGraphWeightCollection::setValue(u16 index, bool value)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types_[index] == AnimationBlendInputType::Bool);
#endif
    POMDOG_ASSERT(index < parameters_.size());
    parameters_[index].boolValue = value;
}

f32 AnimationGraphWeightCollection::getFloatAtIndex(u16 index) const
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types_[index] == AnimationBlendInputType::Float);
#endif
    POMDOG_ASSERT(index < parameters_.size());
    return parameters_[index].floatValue;
}

bool AnimationGraphWeightCollection::getBoolAtIndex(u16 index) const
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types_[index] == AnimationBlendInputType::Bool);
#endif
    POMDOG_ASSERT(index < parameters_.size());
    return parameters_[index].boolValue;
}

void AnimationGraphWeightCollection::reserve(std::size_t size)
{
    POMDOG_ASSERT(size <= std::numeric_limits<u16>::max());
    parameters_.reserve(size);
}

u16 AnimationGraphWeightCollection::getCount() const noexcept
{
    return static_cast<u16>(parameters_.size());
}

} // namespace pomdog::skeletal2d::detail
