//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLELOADER_D0266408_F310_4DD3_B852_3AF14EA7737B_HPP
#define POMDOG_PARTICLELOADER_D0266408_F310_4DD3_B852_3AF14EA7737B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class ParticleClip;

namespace Details {

struct ParticleLoader {
	static ParticleClip LoadFromJson(AssetManager & assets, std::string const& assetPath);

	static ParticleClip Load(AssetManager & assets, std::string const& assetPath);
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLELOADER_D0266408_F310_4DD3_B852_3AF14EA7737B_HPP)
