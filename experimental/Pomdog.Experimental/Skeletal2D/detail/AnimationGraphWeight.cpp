//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "AnimationGraphWeight.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {
//-----------------------------------------------------------------------
AnimationGraphWeight::AnimationGraphWeight(float value)
	: type(AnimationBlendInputType::Float)
{
	SetValue(value);
}
//-----------------------------------------------------------------------
AnimationGraphWeight::AnimationGraphWeight(bool value)
	: type(AnimationBlendInputType::Bool)
{
	SetValue(value);
}
//-----------------------------------------------------------------------
void AnimationGraphWeight::SetValue(float value)
{
	POMDOG_ASSERT(type == AnimationBlendInputType::Float);
	weight = value;
}
//-----------------------------------------------------------------------
void AnimationGraphWeight::SetValue(bool value)
{
	POMDOG_ASSERT(type == AnimationBlendInputType::Bool);
	weight = (value ? 1.0f: 0.0f);
}
//-----------------------------------------------------------------------
float AnimationGraphWeight::GetFloat() const
{
	POMDOG_ASSERT(type == AnimationBlendInputType::Float);
	return weight;
}
//-----------------------------------------------------------------------
bool AnimationGraphWeight::GetBool() const
{
	POMDOG_ASSERT(type == AnimationBlendInputType::Bool);
	return weight != 0.0f;
}
//-----------------------------------------------------------------------
}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog
