//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ASSETMANAGER_FAF5E4A4_3E1E_4A97_BA13_57E85E5B9156_HPP
#define POMDOG_ASSETMANAGER_FAF5E4A4_3E1E_4A97_BA13_57E85E5B9156_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/AssetLoader.hpp"
#include "detail/AssetDictionary.hpp"
#include "detail/EffectLoader.hpp"
#include "detail/TextureLoader.hpp"
#include <utility>

namespace Pomdog {
namespace Details {

template <typename T> struct AssetManagerLoadImpl {
	static auto Load(Details::AssetLoaderContext const& loaderContext,
		std::string const& assetPath,
		Details::AssetDictionary<EffectPass> & effectPassAssets,
		Details::AssetDictionary<Texture2D> & texture2dAssets)
		-> decltype(Details::AssetLoader<T>()(Details::AssetLoaderContext(), std::string()))
	{
		Details::AssetLoader<T> loader;
		return loader(loaderContext, assetPath);
	}
};

template <> struct AssetManagerLoadImpl<Texture2D> {
	static auto Load(Details::AssetLoaderContext const& loaderContext,
		std::string const& assetPath,
		Details::AssetDictionary<EffectPass> & effectPassAssets,
		Details::AssetDictionary<Texture2D> & texture2dAssets)
	{
		return texture2dAssets.Load(loaderContext, assetPath);
	}
};

template <> struct AssetManagerLoadImpl<EffectPass> {
	static auto Load(Details::AssetLoaderContext const& loaderContext,
		std::string const& assetPath,
		Details::AssetDictionary<EffectPass> & effectPassAssets,
		Details::AssetDictionary<Texture2D> & texture2dAssets)
	{
		return effectPassAssets.Load(loaderContext, assetPath);
	}
};

}// namespace Details

///@~Japanese
/// @brief アセットを読み込みます。
class AssetManager {
public:
	AssetManager(Details::AssetLoaderContext && loaderContext);

	///@~Japanese
	/// @brief アセットを読み込みます。
	template <typename T>
	auto Load(std::string const& assetPath)-> decltype(Details::AssetLoader<T>()(Details::AssetLoaderContext(), std::string()))
	{
		return Details::AssetManagerLoadImpl<T>::Load(loaderContext, assetPath,
			effectPassAssets, texture2dAssets);
	}
	
	std::string RootDirectory() const
	{
		return loaderContext.RootDirectory;
	}
	
	///@~Japanese
	/// @brief 読み込まれたすべてのアセットを破棄します。
	void Unload();
	
private:
	Details::AssetLoaderContext loaderContext;
	Details::AssetDictionary<EffectPass> effectPassAssets;
	Details::AssetDictionary<Texture2D> texture2dAssets;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ASSETMANAGER_FAF5E4A4_3E1E_4A97_BA13_57E85E5B9156_HPP)
