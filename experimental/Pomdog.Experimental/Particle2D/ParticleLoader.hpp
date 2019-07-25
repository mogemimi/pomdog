// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class ParticleClip;

namespace Detail {

struct ParticleLoader final {
    static ParticleClip LoadFromJson(AssetManager & assets, std::string const& assetName);

    static ParticleClip Load(AssetManager & assets, std::string const& assetName);
};

} // namespace Detail
} // namespace Pomdog
