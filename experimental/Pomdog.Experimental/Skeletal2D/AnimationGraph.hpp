//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ANIMATIONGRAPH_CFF1C0A9_DB7B_4B31_B1A9_F54EB357E33B_HPP
#define POMDOG_ANIMATIONGRAPH_CFF1C0A9_DB7B_4B31_B1A9_F54EB357E33B_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "AnimationBlendInput.hpp"
#include "AnimationNode.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <algorithm>

namespace Pomdog {

class AnimationGraphState {
public:
	std::string Name;
	std::unique_ptr<AnimationNode> Tree;
};

class AnimationGraph {
public:
	std::vector<AnimationGraphState> States;
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
