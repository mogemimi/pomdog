// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_AUDIOCLIPLOADER_18B28F6C_03C2_407A_8514_38A44B778177_HPP
#define POMDOG_AUDIOCLIPLOADER_18B28F6C_03C2_407A_8514_38A44B778177_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Content/detail/AssetLoader.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <memory>

namespace Pomdog {

class AudioClip;

namespace Detail {

class AssetLoaderContext;

template <>
struct POMDOG_EXPORT AssetLoader<AudioClip> final {
	std::shared_ptr<AudioClip> operator()(
		AssetLoaderContext const& loaderContext,
		std::string const& assetName);
};

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOCLIPLOADER_18B28F6C_03C2_407A_8514_38A44B778177_HPP)
