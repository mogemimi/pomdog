// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP
#define POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP

#include "Behavior.hpp"
//#include <selene.h>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class ScriptBehavior: public Behavior {
public:
    ScriptBehavior(AssetManager const& assets, std::string const& filePath);

    void Start(GameObject & gameObject) override;

    void Update(GameObject & gameObject, Duration const& frameDuration) override;

private:
    //sel::State state;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SCRIPTBEHAVIOR_BBF00E47_6D84_4C29_9BF3_C8C352F26252_HPP)
