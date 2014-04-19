//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKELETON2DLOADER_24651167_613E_4D78_A91B_BBF0AD197E19_HPP
#define POMDOG_SKELETON2DLOADER_24651167_613E_4D78_A91B_BBF0AD197E19_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "SkeletonDesc.hpp"

namespace Pomdog {
namespace Details {

template <>
struct AssetLoader<Skeletal2D::SkeletonDesc> {
	Skeletal2D::SkeletonDesc operator()(AssetLoaderContext const& loaderContext,
		std::string const& assetPath);
};

extern template struct AssetLoader<Skeletal2D::SkeletonDesc>;

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SKELETON2DLOADER_24651167_613E_4D78_A91B_BBF0AD197E19_HPP)
