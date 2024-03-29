// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

std::uint16_t AnimationGraphWeightCollection::AddFloat()
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types.size() == parameters.size());
    types.push_back(AnimationBlendInputType::Float);
#endif
    Weight weight;
    weight.floatValue = 0.0f;
    parameters.push_back(std::move(weight));
    return static_cast<std::uint16_t>(parameters.size()) - 1;
}

std::uint16_t AnimationGraphWeightCollection::AddBool()
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types.size() == parameters.size());
    types.push_back(AnimationBlendInputType::Bool);
#endif
    Weight weight;
    weight.boolValue = false;
    parameters.push_back(std::move(weight));
    return static_cast<std::uint16_t>(parameters.size()) - 1;
}

void AnimationGraphWeightCollection::SetValue(std::uint16_t index, float value)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types[index] == AnimationBlendInputType::Float);
#endif
    POMDOG_ASSERT(index < parameters.size());
    parameters[index].floatValue = value;
}

void AnimationGraphWeightCollection::SetValue(std::uint16_t index, bool value)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types[index] == AnimationBlendInputType::Bool);
#endif
    POMDOG_ASSERT(index < parameters.size());
    parameters[index].boolValue = value;
}

float AnimationGraphWeightCollection::GetFloatAtIndex(std::uint16_t index) const
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types[index] == AnimationBlendInputType::Float);
#endif
    POMDOG_ASSERT(index < parameters.size());
    return parameters[index].floatValue;
}

bool AnimationGraphWeightCollection::GetBoolAtIndex(std::uint16_t index) const
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(types[index] == AnimationBlendInputType::Bool);
#endif
    POMDOG_ASSERT(index < parameters.size());
    return parameters[index].boolValue;
}

void AnimationGraphWeightCollection::Reserve(std::size_t size)
{
    POMDOG_ASSERT(size <= std::numeric_limits<std::uint16_t>::max());
    parameters.reserve(size);
}

std::uint16_t AnimationGraphWeightCollection::GetCount() const noexcept
{
    return static_cast<std::uint16_t>(parameters.size());
}

} // namespace pomdog::skeletal2d::detail
