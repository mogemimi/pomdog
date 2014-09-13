//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationGraphWeightCollection.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
//-----------------------------------------------------------------------
std::uint16_t AnimationGraphWeightCollection::AddFloat()
{
	parameters.emplace_back(0.0f);
	return parameters.size() - 1;
}
//-----------------------------------------------------------------------
std::uint16_t AnimationGraphWeightCollection::AddBool()
{
	parameters.emplace_back(false);
	return parameters.size() - 1;
}
//-----------------------------------------------------------------------
void AnimationGraphWeightCollection::SetValue(std::uint16_t index, float value)
{
	POMDOG_ASSERT(index < parameters.size());
	parameters[index].SetValue(value);
}
//-----------------------------------------------------------------------
void AnimationGraphWeightCollection::SetValue(std::uint16_t index, bool value)
{
	POMDOG_ASSERT(index < parameters.size());
	parameters[index].SetValue(value);
}
//-----------------------------------------------------------------------
AnimationGraphWeight const& AnimationGraphWeightCollection::At(std::uint16_t index) const
{
	POMDOG_ASSERT(index < parameters.size());
	return parameters[index];
}
//-----------------------------------------------------------------------
void AnimationGraphWeightCollection::Reserve(std::uint16_t size)
{
	parameters.reserve(size);
}
//-----------------------------------------------------------------------
std::uint16_t AnimationGraphWeightCollection::Count() const
{
	return parameters.size();
}
//-----------------------------------------------------------------------
}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
