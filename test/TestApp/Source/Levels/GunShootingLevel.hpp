// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_GUNSHOOTINGLEVEL_2CB60E26_HPP
#define POMDOG_GUNSHOOTINGLEVEL_2CB60E26_HPP

#include <Pomdog.Experimental/Experimental.hpp>
#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class GunShootingLevel final: public GameLevel {
public:
    GunShootingLevel(GameHost & gameHost, GameWorld & world);

    void Update(GameHost & gameHost, GameWorld & world) override;

private:
    GameObject mainCamera;
    GameObject maid;
    GameObject lightningBeam;
    Duration spawnTime;

    std::shared_ptr<Skeleton> ghostSkeleton;
    std::shared_ptr<AnimationGraph> ghostAnimGraph;
    std::shared_ptr<Texture2D> ghostTexture;
    std::shared_ptr<SkinnedMesh> ghostMesh;
};

}// namespace TestApp

#endif // POMDOG_GUNSHOOTINGLEVEL_2CB60E26_HPP
