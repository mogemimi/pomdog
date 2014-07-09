﻿//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONGRAPH_CFF1C0A9_DB7B_4B31_B1A9_F54EB357E33B_HPP
#define POMDOG_ANIMATIONGRAPH_CFF1C0A9_DB7B_4B31_B1A9_F54EB357E33B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <memory>
#include <vector>
#include <algorithm>
#include <Pomdog/Utility/Optional.hpp>
#include "AnimationBlendInput.hpp"

namespace Pomdog {

class AnimationGraph {
public:
	std::unique_ptr<AnimationNode> Tree;
	std::vector<AnimationBlendInput> Inputs;
	
	Optional<std::uint16_t> FindParameter(std::string const& name) const
	{
		auto iter = std::find_if(std::begin(Inputs), std::end(Inputs), [&name](AnimationBlendInput const& input) {
			return input.Name == name;
		});
		
		if (iter != std::end(Inputs)) {
			return std::distance(std::begin(Inputs), iter);
		}
		return OptionalType::NullOptional;
	}
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONGRAPH_CFF1C0A9_DB7B_4B31_B1A9_F54EB357E33B_HPP)
