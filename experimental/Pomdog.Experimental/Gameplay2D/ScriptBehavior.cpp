// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "ScriptBehavior.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
ScriptBehavior::ScriptBehavior(AssetManager const& assets, std::string const& filePath)
    //: state{true}
{
    //state.Load(assets.RootDirectory() + "/" + filePath);
}
//-----------------------------------------------------------------------
void ScriptBehavior::Start(GameObject & gameObject)
{
}
//-----------------------------------------------------------------------
void ScriptBehavior::Update(GameObject & gameObject, Duration const& frameDuration)
{
//    auto transform = gameObject.Component<Transform2D>();
//
//    state["Transform"]["Position"]["X"] = static_cast<double>(transform->Position.X);
//    state["Transform"]["Position"]["Y"] = static_cast<double>(transform->Position.Y);
//    state["Transform"]["Rotation"] = static_cast<double>(transform->Rotation.value);
//
//    POMDOG_ASSERT(state["Update"]);
//    state["Update"](static_cast<float>(frameDuration.count()));
//
//    transform->Position.X = static_cast<double>(state["Transform"]["Position"]["X"]);
//    transform->Position.Y = static_cast<double>(state["Transform"]["Position"]["Y"]);
//    transform->Rotation = static_cast<double>(state["Transform"]["Rotation"]);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
