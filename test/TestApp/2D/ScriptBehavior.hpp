//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP
#define POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <selene.h>
#include <Pomdog/Pomdog.hpp>
#include "Behavior.hpp"

namespace Pomdog {

class ScriptBehavior: public Behavior {
public:
	ScriptBehavior(AssetManager const& assets, std::string const& filePath)
		: state{true}
	{
		state.Load(assets.RootDirectory() + "/" + filePath);
	}

	void Start(GameObject & gameObject) override
	{
	}

	void Update(GameObject & gameObject, DurationSeconds const& frameDuration) override
	{
		auto transform = gameObject.Component<Transform2D>();
	
		//state["Transform"]["Position"]["X"].SetObj(transform->Position, "X", &Vector2::X);
		state["Transform"]["Position"]["X"] = static_cast<double>(transform->Position.X);
		state["Transform"]["Position"]["Y"] = static_cast<double>(transform->Position.Y);
		state["Transform"]["Rotation"] = static_cast<double>(transform->Rotation.value);
	
		POMDOG_ASSERT(state["Update"]);
		state["Update"](static_cast<float>(frameDuration.count()));

		transform->Position.X = static_cast<double>(state["Transform"]["Position"]["X"]);
		transform->Position.Y = static_cast<double>(state["Transform"]["Position"]["Y"]);
		transform->Rotation = static_cast<double>(state["Transform"]["Rotation"]);
	}
	
private:
	sel::State state;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP)
