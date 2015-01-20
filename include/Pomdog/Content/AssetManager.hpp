//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ASSETMANAGER_FAF5E4A4_3E1E_4A97_BA13_57E85E5B9156_HPP
#define POMDOG_ASSETMANAGER_FAF5E4A4_3E1E_4A97_BA13_57E85E5B9156_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/AssetLoaderContext.hpp"
#include "detail/AssetDictionary.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT AssetManager {
public:
	AssetManager(Details::AssetLoaderContext && loaderContext);

	///@~Japanese
	/// @brief アセットを読み込みます。
	template <typename T>
	std::shared_ptr<T> Load(std::string const& assetName)
	{
		return assets.Load<T>(loaderContext, assetName);
	}

	///@~Japanese
	/// @brief 読み込まれたすべてのアセットを破棄します。
	void Unload();

	std::string RootDirectory() const;

	std::ifstream OpenStream(std::string const& assetName) const;

private:
	Details::AssetLoaderContext loaderContext;
	Details::AssetDictionary assets;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ASSETMANAGER_FAF5E4A4_3E1E_4A97_BA13_57E85E5B9156_HPP)
