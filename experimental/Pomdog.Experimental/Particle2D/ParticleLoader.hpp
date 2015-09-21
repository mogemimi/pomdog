// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLELOADER_D0266408_HPP
#define POMDOG_PARTICLELOADER_D0266408_HPP

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class ParticleClip;

namespace Detail {

struct ParticleLoader {
    static ParticleClip LoadFromJson(AssetManager & assets, std::string const& assetName);

    static ParticleClip Load(AssetManager & assets, std::string const& assetName);
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PARTICLELOADER_D0266408_HPP
