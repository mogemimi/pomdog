//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLELOADER_D0266408_F310_4DD3_B852_3AF14EA7737B_HPP
#define POMDOG_PARTICLELOADER_D0266408_F310_4DD3_B852_3AF14EA7737B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class ParticleClip;

namespace Detail {

struct ParticleLoader {
	static ParticleClip LoadFromJson(AssetManager & assets, std::string const& assetName);

	static ParticleClip Load(AssetManager & assets, std::string const& assetName);
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLELOADER_D0266408_F310_4DD3_B852_3AF14EA7737B_HPP)
