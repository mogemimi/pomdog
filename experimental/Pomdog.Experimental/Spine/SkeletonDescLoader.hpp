//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPINE_SKELETON2DLOADER_24651167_613E_4D78_A91B_BBF0AD197E19_HPP
#define POMDOG_SPINE_SKELETON2DLOADER_24651167_613E_4D78_A91B_BBF0AD197E19_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "SkeletonDesc.hpp"
#include "Pomdog/Content/detail/AssetLoader.hpp"

namespace Pomdog {
namespace Details {

template <>
struct AssetLoader<Spine::SkeletonDesc> {
	Spine::SkeletonDesc operator()(AssetLoaderContext const& loaderContext,
		std::string const& assetPath);
};

extern template struct AssetLoader<Spine::SkeletonDesc>;

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPINE_SKELETON2DLOADER_24651167_613E_4D78_A91B_BBF0AD197E19_HPP)
