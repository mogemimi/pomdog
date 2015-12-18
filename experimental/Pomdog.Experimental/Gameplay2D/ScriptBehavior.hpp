// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SCRIPTBEHAVIOR_BBF00E47_HPP
#define POMDOG_SCRIPTBEHAVIOR_BBF00E47_HPP

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

} // namespace Pomdog

#endif // POMDOG_SCRIPTBEHAVIOR_BBF00E47_HPP
