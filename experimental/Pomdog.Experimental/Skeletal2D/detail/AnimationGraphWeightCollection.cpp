// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "AnimationGraphWeightCollection.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <limits>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

std::uint16_t AnimationGraphWeightCollection::AddFloat()
{
    parameters.emplace_back(0.0f);
    return parameters.size() - 1;
}

std::uint16_t AnimationGraphWeightCollection::AddBool()
{
    parameters.emplace_back(false);
    return parameters.size() - 1;
}

void AnimationGraphWeightCollection::SetValue(std::uint16_t index, float value)
{
    POMDOG_ASSERT(index < parameters.size());
    parameters[index].SetValue(value);
}

void AnimationGraphWeightCollection::SetValue(std::uint16_t index, bool value)
{
    POMDOG_ASSERT(index < parameters.size());
    parameters[index].SetValue(value);
}

AnimationGraphWeight const& AnimationGraphWeightCollection::At(std::uint16_t index) const
{
    POMDOG_ASSERT(index < parameters.size());
    return parameters[index];
}

void AnimationGraphWeightCollection::Reserve(std::uint16_t size)
{
    parameters.reserve(size);
}

void AnimationGraphWeightCollection::Reserve(std::size_t size)
{
    POMDOG_ASSERT(size <= std::numeric_limits<std::uint16_t>::max());
    parameters.reserve(size);
}

std::uint16_t AnimationGraphWeightCollection::Count() const
{
    return parameters.size();
}

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
