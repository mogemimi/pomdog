//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP
#define POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

//#include <selene.h>
#include <Pomdog/Pomdog.hpp>
#include "Behavior.hpp"

namespace Pomdog {

class ScriptBehavior: public Behavior {
public:
	ScriptBehavior(AssetManager const& assets, std::string const& filePath);

	void Start(GameObject & gameObject) override;

	void Update(GameObject & gameObject, DurationSeconds const& frameDuration) override;

private:
	//sel::State state;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP)
