//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ANIMATIONGRAPHWEIGHTCOLLECTION_5506C893_2829_4E01_8EE5_72B642FF79AB_HPP
#define POMDOG_ANIMATIONGRAPHWEIGHTCOLLECTION_5506C893_2829_4E01_8EE5_72B642FF79AB_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "AnimationGraphWeight.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class AnimationGraphWeightCollection {
public:
	void SetValue(std::uint16_t index, float value);
	void SetValue(std::uint16_t index, bool value);

	AnimationGraphWeight const& At(std::uint16_t index) const;

	std::uint16_t AddFloat();
	std::uint16_t AddBool();

	std::uint16_t Count() const;

	void Reserve(std::uint16_t size);

	void Reserve(std::size_t size);

private:
	std::vector<AnimationGraphWeight> parameters;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONGRAPHWEIGHTCOLLECTION_5506C893_2829_4E01_8EE5_72B642FF79AB_HPP)
