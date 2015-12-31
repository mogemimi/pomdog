// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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
