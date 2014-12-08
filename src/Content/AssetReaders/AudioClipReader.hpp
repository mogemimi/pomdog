//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_AUDIOCLIPREADER_AAF2CAB2_CE84_449D_BB06_1B89C71AA848_HPP
#define POMDOG_AUDIOCLIPREADER_AAF2CAB2_CE84_449D_BB06_1B89C71AA848_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <string>
#include <memory>

namespace Pomdog {

class AudioClip;

namespace Details {

class AssetLoaderContext;

class AudioClipReader final {
public:
	static std::shared_ptr<AudioClip> Read(
		AssetLoaderContext const& loaderContext, std::string const& assetName);
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOCLIPREADER_AAF2CAB2_CE84_449D_BB06_1B89C71AA848_HPP)
