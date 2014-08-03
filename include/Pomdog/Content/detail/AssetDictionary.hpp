//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP
#define POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AssetLoader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace Pomdog {
namespace Details {

template <class T>
class AssetDictionary {
public:
	std::unordered_map<std::string, std::weak_ptr<T>> assets;

	std::shared_ptr<T> Load(AssetLoaderContext const& loaderContext, std::string const& assetPath)
	{
		auto const iter = assets.find(assetPath);
		if (iter != std::end(assets))
		{
			if (auto asset = iter->second.lock()) {
				return asset;
			}
			
			POMDOG_ASSERT(iter->second.expired());
			assets.erase(iter);
		}

		auto asset = ReadAsset(loaderContext, assetPath);

		POMDOG_ASSERT(asset);
		assets.emplace(assetPath, asset);
		return asset;
	}

	void Unload()
	{
		assets.clear();
	}

	std::shared_ptr<T> ReadAsset(AssetLoaderContext const& loaderContext, std::string const& assetPath)
	{
		Details::AssetLoader<T> loader;
		return loader(loaderContext, assetPath);
	}
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP)
