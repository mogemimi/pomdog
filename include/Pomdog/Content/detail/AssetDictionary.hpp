//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP
#define POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Utility/detail/Any.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <string>
#include <memory>

namespace Pomdog {
namespace Details {

class AssetLoaderContext;
class AssetReader;

class POMDOG_EXPORT AssetDictionary final {
private:
	std::unordered_map<std::string, Any> assets;
	std::unordered_map<std::type_index, std::unique_ptr<AssetReader>> readers;

public:
	AssetDictionary();
	
	~AssetDictionary();

	template <typename T>
	std::shared_ptr<T> Load(AssetLoaderContext const& loaderContext, std::string const& assetName)
	{
		static_assert(std::is_object<T>::value, "");
		
		std::type_index const typeIndex = typeid(std::shared_ptr<T>);
		
		auto const iter = assets.find(assetName);
		if (iter != std::end(assets))
		{
			auto & assetHolder = iter->second;
			
			if (assetHolder.Type() == typeIndex) {
				return assetHolder.As<std::shared_ptr<T>>();
			}

			assets.erase(iter);
		}

		auto assetHolder = ReadAsset(loaderContext, assetName, typeIndex);
		
		POMDOG_ASSERT(assetHolder.Type() == typeIndex);
		
		if (assetHolder.Type() != typeIndex) {
			/// FUS RO DAH!
			///@todo Not implemented
			//throw exception
		}
		
		auto asset = assetHolder.As<std::shared_ptr<T>>();
		assets.emplace(assetName, std::move(assetHolder));
		
		return std::move(asset);
	}

	void Unload();

private:
	Any ReadAsset(AssetLoaderContext const& loaderContext,
		std::string const& assetName, std::type_index const& typeIndex) const;
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ASSETDICTIONARY_8088231B_F800_4D65_A75E_0160C4ED42B8_HPP)
